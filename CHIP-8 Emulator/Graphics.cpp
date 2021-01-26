#include "Graphics.h"
#include <wx/msgdlg.h>


Graphics::Graphics()
{
}


Graphics::~Graphics()
{
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
}


void Graphics::Initialize(SDL_Window* SDL_window)
{
	this->window = SDL_window;

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == NULL)
	{
		wxMessageBox("Renderer could not be created!");
		return;
	}
}


void Graphics::RenderGraphics()
{
	for (int pixel = 0; pixel < 64 * 32; pixel++)
	{
		if (chip8->gfx[pixel])
			SDL_SetRenderDrawColor(renderer, colourOn.Red(), colourOn.Green(), colourOn.Blue(), colourOn.Alpha());
		else
			SDL_SetRenderDrawColor(renderer, colourOff.Red(), colourOff.Green(), colourOff.Blue(), colourOff.Alpha());

		int col = pixel % 64;
		int row = pixel / 64;
		rect.x = col * scale + xPixelOffset;
		rect.y = row * scale + yPixelOffset;
		SDL_RenderFillRect(renderer, &rect);
	}

	SDL_RenderPresent(renderer);
}


void Graphics::SetColour(wxColour colour, bool state, bool emulatorIsRunning)
{
	if (state)
		colourOn = colour;
	else
		colourOff = colour;

	if (emulatorIsRunning)
	{
		Clear();
		RenderGraphics();
	}
}


void Graphics::ComputeScaleAndOffsets(int width, int height)
{
	// calculate the scale and pixel offsets given a new width and height
	scale = std::min(width / 64, height / 32);
	xPixelOffset = 0.5 * (width - scale * 64);
	yPixelOffset = 0.5 * (height - scale * 32);
}


void Graphics::SetWindowSize(int width, int height, bool emulatorIsRunning)
{
	ComputeScaleAndOffsets(width, height);
	rect.w = rect.h = scale;
	if (emulatorIsRunning)
	{
		DrawBorder();
		Clear();
		RenderGraphics();
	}
}


void Graphics::Clear()
{
	SDL_SetRenderDrawColor(renderer, colourOff.Red(), colourOff.Green(), colourOff.Blue(), colourOff.Alpha());
	rect.x = xPixelOffset;
	rect.y = yPixelOffset;
	rect.w = 64 * scale;
	rect.h = 32 * scale;
	SDL_RenderFillRect(renderer, &rect);
	SDL_RenderPresent(renderer);

	rect.w = rect.h = scale;
}


void Graphics::DrawBorder()
{
	SDL_SetRenderDrawColor(renderer, colourBorder.Red(), colourBorder.Green(), colourBorder.Blue(), colourBorder.Alpha());
	SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);
}