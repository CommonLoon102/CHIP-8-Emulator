#include "Input.h"


Input::Input()
{
}


Input::~Input()
{
}


// poll key presses. gets called in every emulation cycle
std::tuple<u8, bool> Input::PollInput()
{
	u8 key;
	if (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_KEYDOWN:
			key = TranslateKey(event.key.keysym.sym);
			return std::tuple<u8, bool>{key, true};

		case SDL_KEYUP:
			key = TranslateKey(event.key.keysym.sym);
			return std::tuple<u8, bool>{key, false};

		default:
			return std::tuple<u8, bool>{UNMAPPED_KEY, false};
		}
	}
	return std::tuple<u8, bool>{UNMAPPED_KEY, false};
}


// await a key press (blocking operation). gets called only when decoding a specific opcode
u8 Input::AwaitInput()
{
	u8 key;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_KEYDOWN:
			key = TranslateKey(event.key.keysym.sym);
			if (key != UNMAPPED_KEY)
				return key;
			else
				continue;
			
		default:
			continue;
		}
	}
	return UNMAPPED_KEY;
}


// translate keyCode (pressed key) to the corresponding key on the chip-8 keypad
// return 0xFF if none of the mapped keys were pressed
u8 Input::TranslateKey(SDL_Keycode keyCode)
{
	/* The set of keys below (left) is set to correspond to the chip-8 keypad (right)
	1 2 3 4		1 2 3 C
	Q W E R		4 5 6 D
	A S D F		7 8 9 E
	Z X C V		A 0 B F
	*/
	switch (keyCode)
	{
	case SDLK_1:
		return 0x1;
	case SDLK_2:
		return 0x2;
	case SDLK_3:
		return 0x3;
	case SDLK_4:
		return 0xC;
	case SDLK_q:
		return 0x4;
	case SDLK_w:
		return 0x5;
	case SDLK_e:
		return 0x6;
	case SDLK_r:
		return 0xD;
	case SDLK_a:
		return 0x7;
	case SDLK_s:
		return 0x8;
	case SDLK_d:
		return 0x9;
	case SDLK_f:
		return 0xE;
	case SDLK_z:
		return 0xA;
	case SDLK_x:
		return 0x0;
	case SDLK_c:
		return 0xB;
	case SDLK_v:
		return 0xF;
	default:
		return UNMAPPED_KEY; // none of the mapped keys were pressed
	}
}