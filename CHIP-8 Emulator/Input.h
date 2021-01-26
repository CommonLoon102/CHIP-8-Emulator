#pragma once

#include "SDL.h"
#include "Types.h"
#include <tuple>

class Input
{
public:
	Input();
	~Input();

	static const u8 UNMAPPED_KEY = 0xFF;
	std::tuple<u8, bool> PollInput();
	u8 AwaitInput();
	u8 TranslateKey(SDL_Keycode keyCode);

private:
	SDL_Event event;
};