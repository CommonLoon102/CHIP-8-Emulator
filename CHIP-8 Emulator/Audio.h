#pragma once

#include "SDL_mixer.h"

class Audio
{
public:
	Audio();
	~Audio();

	void PlayAudio();
	void SetSoundEffect(int index);

private:
	int index; // the chosen sound effect (0=OFF, 1, 2, or 3)
	Mix_Chunk* soundEffect = nullptr;

	const char* soundEffectFilePaths[3] =
	{
		"..//sounds//beep-01.wav",
		"..//sounds//beep-02.wav",
		"..//sounds//beep-03.wav"
	};
};

