#pragma once

class MenuBarID
{
public:
	// IDs for all buttons in the menubar
	static enum ID
	{
		open_rom = 10003,
		game_dir,
		save_state,
		load_state,
		quit,
		pause_play,
		reset,
		stop,
		size_5x = 5,
		size_10x = 10,
		size_15x = 15,
		size_20x = 20,
		size_30x = 30,
		size_40x = 40,
		size_custom,
		size_fullscreen,
		speed_250 = 250,
		speed_500 = 500,
		speed_750 = 750,
		speed_1000 = 1000,
		speed_1250 = 1250,
		speed_1500 = 1500,
		speed_custom,
		colour_off,
		colour_on,
		sound_off,
		sound_1,
		sound_2,
		sound_3,
		reset_settings,
		github_link,
		listBoxDoubleClick
	};

	static bool isSpeedID(int id);
	static bool isSizeID(int id);
};

