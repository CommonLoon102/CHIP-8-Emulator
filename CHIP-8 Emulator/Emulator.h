#pragma once

#include "Chip8.h"
#include "Graphics.h"
#include "Input.h"
#include "Audio.h"
#include "wx/wx.h"
#include <wx/stdpaths.h>
#include <wx/filename.h>

class Emulator
{
public:
	Emulator();
	~Emulator();

	bool initialized = false;
	bool isRunning = false;
	bool isPaused = false;

	const int framesPerSecond = 60;
	const int microsecondsPerFrame = (int)(1e6 / (double)framesPerSecond);
	int cyclesPerSecond;
	int cyclesPerFrame;

	void Initialize(SDL_Window* SDL_window);
	void StartGame(wxString pathToRom);
	void Pause();
	void Resume();
	void Reset();
	void Stop();

	void SetSpeed(int cyclesPerSecond);
	void SetWindowSize(int width, int height);
	void SetColour(wxColour colour, bool state);
	void SetSoundEffect(int soundEffect);

	void SaveState();
	void LoadState();

private:
	const wxString SAVESTATE_PATH = wxFileName(wxStandardPaths::Get().GetExecutablePath())
		.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR) + "state.bin";

	Audio audio;
	Graphics graphics;

	// need to be shared with other classes
	Chip8* chip8 = nullptr;
	Input* input = nullptr;
	
	void MainLoop();
};

