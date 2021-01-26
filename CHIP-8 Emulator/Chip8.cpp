#include "Chip8.h"
#include <wx/msgdlg.h>
#include <chrono>
#include <iostream>


Chip8::Chip8()
{
	Reset(true);
}


Chip8::~Chip8()
{
}


// the resetMemory flag is needed because we may want to reset the system but not the loaded rom or fontset
void Chip8::Reset(bool resetMemory)
{
	pc = 0x200;
	I = 0;
	sp = 0;
	drawFlag = false;

	for (int i = 0; i < 64 * 32; i++) // clear the frame buffer
		gfx[i] = 0;

	for (u8 i = 0; i < 16; i++) // clear the registers and the stack
		V[i] = stack[i] = 0;

	if (resetMemory)
	{
		for (u16 i = 0x000; i <= 0xFFF; i++) // clear the memory
			memory[i] = 0;

		for (u8 i = 0; i < 80; i++) // load the fontset
			memory[i] = fontset[i];
	}

	delayTimer = soundTimer = 60;
	running = true;
}


void Chip8::EmulateCycle()
{
	soundTimerUpdated = delayTimerUpdated = false;

	// fetch opcode
	u16 opcode = memory[pc] << 8 | memory[pc + 1];

	if (drawFlag)
		drawFlag = false; // since at this point, we have already drawn what we were supposed to

	// decode and execute opcode
	DecodeOpcode(opcode);
}


// load the given rom into memory starting from 0x200
bool Chip8::LoadRom(const char* file)
{
	// open the file
	FILE* rom = fopen(file, "rb");
	if (rom == NULL)
	{
		wxMessageBox("Failed to open rom file.");
		return false;
	}

	// get the file's size and check that it's smaller than or equal to 3840 bytes
	fseek(rom, 0, SEEK_END);
	long rom_size = ftell(rom);
	if (rom_size > 0xFFF - 0x200 + 1)
	{
		wxMessageBox("Rom file is larger than the allowed 3840 bytes.");
		return false;
	}
	if (rom_size == 0)
	{
		wxMessageBox("Rom file is empty.");
		return false;
	}

	// read the file and put it in memory starting from 0x200
	rewind(rom);
	unsigned char* buffer = (unsigned char*)malloc(rom_size);
	if (buffer == NULL)
	{
		wxMessageBox("Failed to allocate memory for rom file.");
		return false;
	}
	fread(buffer, 1, rom_size, rom);
	fclose(rom);
	for (u16 addr = 0x0; addr < rom_size; addr++)
		memory[addr + 0x200] = buffer[addr];
	free(buffer);

	return true;
}


// decode and execute the given opcode
void Chip8::DecodeOpcode(u16 opcode)
{
	u16 x, y, height, pixel;

	switch (opcode & 0xF000)
	{
	case 0x0000:

		switch (opcode)
		{
		case 0x00E0:
			for (int i = 0; i < 64 * 32; i++)
				gfx[i] = 0;
			drawFlag = true;
			pc += 2;
			break;

		case 0x00EE:
			pc = stack[--sp] + 2;
			break;

		default: // 0x0NNN
			wxMessageBox(wxString::Format("Unknown opcode: %i", opcode));
			break;
		}
		break;

	case 0x1000:
		pc = opcode & 0x0FFF;
		break;

	case 0x2000:
		stack[sp++] = pc;
		pc = opcode & 0x0FFF;
		break;

	case 0x3000:
		if (V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF))
			pc += 4;
		else
			pc += 2;
		break;

	case 0x4000:
		if (V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
			pc += 4;
		else
			pc += 2;
		break;

	case 0x5000:
		if (V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4])
			pc += 4;
		else
			pc += 2;
		break;

	case 0x6000:
		V[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
		pc += 2;
		break;

	case 0x7000:
		V[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
		pc += 2;
		break;

	case 0x8000: // 8XYN
		x = (opcode & 0x0F00) >> 8;
		y = (opcode & 0x00F0) >> 4;
		switch (opcode & 0x000F)
		{
		case 0x0000:
			V[x] = V[y];
			break;

		case 0x0001:
			V[x] |= V[y];
			break;

		case 0x0002:
			V[x] &= V[y];
			break;

		case 0x0003:
			V[x] ^= V[y];
			break;

		case 0x0004:
			if (V[y] > 0xFF - V[x])
				V[0xF] = 1;
			else
				V[0xF] = 0;
			V[x] += V[y];
			break;

		case 0x0005:
			if (V[y] > V[x])
				V[0xF] = 0;
			else
				V[0xF] = 1;
			V[x] -= V[y];
			break;

		case 0x0006:
			V[0xF] = V[x] & 0x01;
			V[x] >>= 1;
			break;

		case 0x0007:
			if (V[y] > V[x])
				V[0xF] = 0;
			else
				V[0xF] = 1;
			V[x] = V[y] - V[x];
			break;

		case 0x000E:
			V[0xF] = (V[x] & 0x10) >> 1;
			V[x] <<= 1;
			break;

		default:
			wxMessageBox(wxString::Format("Unknown opcode: %i", opcode));
			break;
		}
		pc += 2;
		break;

	case 0x9000:
		if (V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4])
			pc += 4;
		else
			pc += 2;
		break;

	case 0xA000:
		I = opcode & 0x0FFF;
		pc += 2;
		break;

	case 0xB000:
		pc = (opcode & 0x0FFF) + V[0x0];
		break;

	case 0xC000:
		V[(opcode & 0x0F00) >> 8] = (rand() % 256) & (opcode & 0x00FF);
		pc += 2;
		break;

	case 0xD000:
		x = V[(opcode & 0x0F00) >> 8];
		y = V[(opcode & 0x00F0) >> 4];
		height = opcode & 0x000F;
		V[0xF] = 0;

		for (int yline = 0; yline < height; yline++)
		{
			pixel = memory[I + yline];
			for (int xline = 0; xline < 8; xline++)
			{
				if ((pixel & (0x80 >> xline)) != 0)
				{
					if (gfx[(x + xline + ((y + yline) * 64)) % (64 * 32)] == 1)
						V[0xF] = 1;
					gfx[(x + xline + ((y + yline) * 64)) % (64 * 32)] ^= 1;
				}
			}
		}

		drawFlag = true;
		pc += 2;
		break;

	case 0xE000:

		switch (opcode & 0x000F)
		{
		case 0x0001:
			if (key[V[(opcode & 0x0F00) >> 8]] != 1)
				pc += 4;
			else
				pc += 2;
			break;

		case 0x000E:
			if (key[V[(opcode & 0x0F00) >> 8]] == 1)
				pc += 4;
			else
				pc += 2;
			break;

		default:
			wxMessageBox(wxString::Format("Unknown opcode: %i", opcode));
			break;
		}
		break;

	case 0xF000:

		switch (opcode & 0x00FF)
		{
		case 0x0007:
			V[(opcode & 0x0F00) >> 8] = delayTimer;
			break;

		case 0x000A:
		{
			// A key press is awaited, and then stored in VX. (Blocking Operation. All instruction halted until next key event)
			u8 _key = input->AwaitInput();
			V[(opcode & 0x0F00) >> 8] = _key;
			SetKey(_key, true);
			break; }

		case 0x0015:
			delayTimer = V[(opcode & 0x0F00) >> 8];
			delayTimerUpdated = true;
			break;

		case 0x0018:
			soundTimer = V[(opcode & 0x0F00) >> 8];
			soundTimerUpdated = true;
			break;

		case 0x001E:
			if (I > 0xFFF - V[(opcode & 0x0F00) >> 8])
				V[0xF] = 1;
			else
				V[0xF] = 0;
			I += V[(opcode & 0x0F00) >> 8];
			break;

		case 0x0029:
			I = V[(opcode & 0x0F00) >> 8] * 5;
			break;

		case 0x0033:
			x = (opcode & 0x0F00) >> 8;
			memory[I] = V[x] / 100;
			memory[I + 1] = (V[x] / 10) % 10;
			memory[I + 2] = (V[x] % 100) % 10;
			break;

		case 0x0055:
			for (auto i = 0x0; i <= (opcode & 0x0F00) >> 8; i++)
				memory[I + i] = V[i];
			break;

		case 0x0065:
			for (auto i = 0x0; i <= (opcode & 0x0F00) >> 8; i++)
				V[i] = memory[I + i];
			break;

		default:
			wxMessageBox(wxString::Format("Unknown opcode: %i", opcode));
			break;
		}
		pc += 2;
		break;
	}
}


void Chip8::SetKey(u8 _key, bool active)
{
	key[_key] = active;
}


// currently does not work correctly
void Chip8::SaveState(std::ofstream& ofs)
{
	//ofs.write((char*)&drawFlag, sizeof(bool));
	//ofs.write((char*)&gfx, sizeof(bool) * 64 * 32);
	//ofs.write((char*)&running, sizeof(bool));
	//ofs.write((char*)&delayTimerUpdated, sizeof(bool));
	//ofs.write((char*)&soundTimerUpdated, sizeof(bool));

	//ofs.write((char*)&delayTimer, sizeof(u8));
	//ofs.write((char*)&soundTimer, sizeof(u8));
	//ofs.write((char*)&key, sizeof(u8) * 16);
	//ofs.write((char*)&memory, sizeof(u8) * 4096);
	//ofs.write((char*)&V, sizeof(u8) * 16);

	//ofs.write((char*)&I, sizeof(u16));
	//ofs.write((char*)&pc, sizeof(u16));
	//ofs.write((char*)&sp, sizeof(u16));
	//ofs.write((char*)&stack, sizeof(u16) * 16);
}


void Chip8::LoadState(std::ifstream& ifs)
{
	//ifs.read((char*)&drawFlag, sizeof(bool));
	//ifs.read((char*)&gfx, sizeof(bool) * 64 * 32);
	//ifs.read((char*)&running, sizeof(bool));
	//ifs.read((char*)&delayTimerUpdated, sizeof(bool));
	//ifs.read((char*)&soundTimerUpdated, sizeof(bool));

	//ifs.read((char*)&delayTimer, sizeof(u8));
	//ifs.read((char*)&soundTimer, sizeof(u8));
	//ifs.read((char*)&key, sizeof(u8) * 16);
	//ifs.read((char*)&memory, sizeof(u8) * 4096);
	//ifs.read((char*)&V, sizeof(u8) * 16);

	//ifs.read((char*)&I, sizeof(u16));
	//ifs.read((char*)&pc, sizeof(u16));
	//ifs.read((char*)&sp, sizeof(u16));
	//ifs.read((char*)&stack, sizeof(u16) * 16);
}


void Chip8::UpdateTimers()
{
	// timers run at 60 Hz no matter the number of cycles per second
	if (delayTimer == 0)
		delayTimer = 60;
	else
		delayTimer--;

	if (soundTimer == 0)
		soundTimer = 60;
	else
		soundTimer--;
}


// if soundTimer == 0, then audio needs to be played
bool Chip8::audioPlayIsPending()
{
	return soundTimer == 0;
}