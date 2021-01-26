#include "Audio.h"

#include <wx/msgdlg.h>


Audio::Audio()
{
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		wxMessageBox(wxString::Format("Failed to initialize audio. SDL_mixer: %s", Mix_GetError()));
		return;
	}
	index = 0;
	soundEffect = nullptr;
}


Audio::~Audio()
{
	if (soundEffect != nullptr)
		Mix_FreeChunk(soundEffect);
	soundEffect = nullptr;

	Mix_CloseAudio();
}


void Audio::PlayAudio()
{
	if (index != 0)
		if (Mix_PlayChannel(-1, soundEffect, 0) == -1)
			wxMessageBox(wxString::Format("Failed to play sound effect file. SDL_mixer: %s", Mix_GetError()));
}


void Audio::SetSoundEffect(int index)
{
	if (soundEffect != nullptr)
		Mix_FreeChunk(soundEffect);

	if (index != 0)
	{
		soundEffect = Mix_LoadWAV(soundEffectFilePaths[index-1]);
		if (soundEffect == nullptr)
		{
			wxMessageBox(wxString::Format("Failed to open sound effect file. SDL_mixer: %s", Mix_GetError()));
			return;
		}
	}

	this->index = index;
}