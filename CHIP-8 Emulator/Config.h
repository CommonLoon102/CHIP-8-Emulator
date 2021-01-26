#pragma once

#include <wx/filename.h>
#include <wx/stdpaths.h>
#include <wx/string.h>
#include "wx/wx.h"


class Config
{
public:
	Config();
	~Config();

	wxString romDir;
	int speed;
	int scale;
	int soundEffect;
	wxColour colourOff;
	wxColour colourOn;

	bool ConfigFileExists();
	void Load();
	void Save();
	void SetDefaults();

private:
	const wxString DEFAULT_ROM_DIR = wxFileName(wxStandardPaths::Get().GetExecutablePath()).GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
	const wxString CONFIG_FILE_PATH = DEFAULT_ROM_DIR + "config.bin";
	const int DEFAULT_SPEED = 750;
	const int DEFAULT_SCALE = 15;
	const int DEFAULT_SOUND_EFFECT = 1;
	const wxColour DEFAULT_COLOR_OFF = { 255, 255, 255 }; // white
	const wxColour DEFAULT_COLOR_ON = { 0, 0, 0 }; // black
};