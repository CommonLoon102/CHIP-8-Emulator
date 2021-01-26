#include "Config.h"
#include <fstream>


Config::Config()
{
}


Config::~Config()
{
}


bool Config::ConfigFileExists()
{
	return wxFileExists(CONFIG_FILE_PATH);
}


void Config::Save()
{
	std::ofstream ofs(CONFIG_FILE_PATH.mb_str(), std::ofstream::out | std::fstream::trunc);
	if (!ofs) // if the file could not be created
	{
		wxMessageBox("Config file could not be created or saved.");
		return;
	}

	// write wxString romDir to config.bin by first writing its length and then its actual data
	size_t size = strlen(romDir);
	ofs.write((char*)&size, sizeof(size));
	ofs.write(romDir.GetData(), size);

	// write the rest of the variables. sizeof(wxColour) == sizeof(colour), where colour is any instance of wxColour
	ofs.write((char*)&speed, sizeof(int));
	ofs.write((char*)&scale, sizeof(int));
	ofs.write((char*)&soundEffect, sizeof(int));
	ofs.write((char*)&colourOff, sizeof(wxColour));
	ofs.write((char*)&colourOn, sizeof(wxColour));
	ofs.close();
}


void Config::Load()
{
	std::ifstream ifs(CONFIG_FILE_PATH.mb_str(), std::ifstream::in);
	if (!ifs) // if the file could not be opened
	{
		wxMessageBox("Config file could not be opened. Reverting to defaults.");
		SetDefaults();
		return;
	}

	// read wxString romDir from config.bin by first reading its length and then its actual data
	size_t size = 0;
	ifs.read((char*)&size, sizeof(size));
	if (size >= 1000)
	{
		wxMessageBox("Error: cannot read config file; rom directory is longer than the maximum allowed 1000 characters.");
		SetDefaults();
		ifs.close();
		return;
	}
	char str[1000]{};
	ifs.read(str, size);
	romDir = wxString(str);

	// read the rest of the variables
	ifs.read((char*)&speed, sizeof(int));
	ifs.read((char*)&scale, sizeof(int));
	ifs.read((char*)&soundEffect, sizeof(int));
	ifs.read((char*)&colourOff, sizeof(wxColour));
	ifs.read((char*)&colourOn, sizeof(wxColour));
	ifs.close();
}


void Config::SetDefaults()
{
	romDir = DEFAULT_ROM_DIR;
	speed = DEFAULT_SPEED;
	scale = DEFAULT_SCALE;
	soundEffect = DEFAULT_SOUND_EFFECT;
	colourOff = DEFAULT_COLOR_OFF;
	colourOn = DEFAULT_COLOR_ON;
	Save();
}