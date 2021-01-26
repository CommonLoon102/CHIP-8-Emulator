#pragma once

#include "Chip8.h"
#include "SDL.h"
#include <wx/wx.h>


class Graphics
{
public:
	Graphics();
	~Graphics();

	Chip8* chip8 = nullptr; // is shared with the Emulator class

	void Clear();
	void Initialize(SDL_Window* SDL_window);
	void RenderGraphics();
	void SetColour(wxColour colour, bool state, bool emulatorIsRunning);
	void SetWindowSize(int width, int height, bool emulatorIsRunning);

private:
	SDL_Rect rect;
	SDL_Renderer* renderer = nullptr;
	SDL_Window* window = nullptr;

	int scale;
	int xPixelOffset = 0;
	int yPixelOffset = 0;
	wxColour colourOff;
	wxColour colourOn;
	const wxColour colourBorder = { 0, 0, 0 };

	void ComputeScaleAndOffsets(int width, int height);
	void DrawBorder();
};