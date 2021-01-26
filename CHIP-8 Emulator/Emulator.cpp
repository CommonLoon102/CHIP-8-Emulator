#include "Emulator.h"
#include <wx/msgdlg.h>

#include <chrono>
#include <fstream>
#include <iostream>
#include <thread>
#include <tuple>


Emulator::Emulator()
{
	chip8 = new Chip8();
	input = new Input();
}

Emulator::~Emulator()
{
	delete chip8;
	delete input;
}


void Emulator::Initialize(SDL_Window* SDL_window)
{
	// initialize the chip8 system, graphics, audio, and input
	chip8->Reset(true);
	chip8->input = this->input;

	graphics.Initialize(SDL_window);
	graphics.chip8 = this->chip8;

	initialized = true;
}


void Emulator::StartGame(wxString pathToRom)
{
	chip8->Reset(true);

	char* path = _strdup(pathToRom.mb_str());
	bool loadSuccess = chip8->LoadRom(path);
	free(path); // _strdup calls malloc()
	if (loadSuccess)
	{
		isRunning = true;
		isPaused = false;
		MainLoop();
	}
}


void Emulator::MainLoop()
{
	std::chrono::time_point<std::chrono::steady_clock> cycle_start_t, cycle_end_t;
	long long microseconds_elapsed = 0;

	graphics.Clear();

	while (isRunning)
	{
		cycle_start_t = std::chrono::steady_clock::now();

		if (isPaused)
			return;

		for (int i = 0; i < cyclesPerFrame; i++)
		{
			chip8->EmulateCycle();
			if (chip8->drawFlag)
				graphics.RenderGraphics();

			// check for key presses
			std::tuple<u8, bool>& keyPress = input->PollInput();
			u8 key = std::get<0>(keyPress);
			if (key != Input::UNMAPPED_KEY) // if a key was pressed
				chip8->SetKey(key, std::get<1>(keyPress));
		}

		// play audio if needed. the timer runs at 60 Hz: same as the number of frames per second
		chip8->UpdateTimers();
		if (chip8->audioPlayIsPending())
			audio.PlayAudio();

		cycle_end_t = std::chrono::steady_clock::now();
		microseconds_elapsed = std::chrono::duration_cast<std::chrono::microseconds>(cycle_end_t - cycle_start_t).count();

		if (microseconds_elapsed < microsecondsPerFrame)
			std::this_thread::sleep_for(std::chrono::microseconds(microsecondsPerFrame - microseconds_elapsed));
	}

	Stop();
}


void Emulator::Pause()
{
	isPaused = true;
	chip8->running = false;
}


void Emulator::Resume()
{
	isPaused = false;
	chip8->running = true;
	MainLoop();
}


void Emulator::Reset()
{
	isPaused = false;
	isRunning = true;
	chip8->running = false; // so as to reset the timers properly
	chip8->Reset(false); // false meaning don't reset the memory state
	MainLoop();
}


void Emulator::Stop()
{
	isRunning = false;
	isPaused = false;
	chip8->running = false;
}


void Emulator::SaveState()
{
	std::ofstream ofs(SAVESTATE_PATH.mb_str(), std::ofstream::out | std::fstream::trunc);
	if (!ofs) // if the file could not be created
	{
		wxMessageBox("Save state could not be created.");
		return;
	}

	chip8->SaveState(ofs);
	ofs.close();
}


void Emulator::LoadState()
{
	std::ifstream ifs(SAVESTATE_PATH.mb_str(), std::ifstream::in);
	if (!ifs) // if the file could not be opened
	{
		wxMessageBox("Save state does not exist or could not be opened.");
		return;
	}

	chip8->LoadState(ifs);
	ifs.close();

	graphics.Clear();
	graphics.RenderGraphics();
}


void Emulator::SetSpeed(int cyclesPerSecond)
{
	this->cyclesPerSecond = cyclesPerSecond;
	cyclesPerFrame = round((double)cyclesPerSecond / (double)framesPerSecond);
}


void Emulator::SetColour(wxColour colour, bool state)
{
	graphics.SetColour(colour, state, this->isRunning);
}


void Emulator::SetSoundEffect(int soundEffect)
{
	audio.SetSoundEffect(soundEffect);
}


void Emulator::SetWindowSize(int width, int height)
{
	graphics.SetWindowSize(width, height, this->isRunning);
}