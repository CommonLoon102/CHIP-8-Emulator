#include "GUI.h"
#include "MenuBarID.h"
#include "SDL.h"
#include <wx/msgdlg.h>
#include <wx/dir.h>
#include <wx/filename.h>
#include <wx/colordlg.h>


wxBEGIN_EVENT_TABLE(GUI, wxFrame)
	EVT_MENU(MenuBarID::ID::open_rom,		GUI::OnMenuOpen)
	EVT_MENU(MenuBarID::ID::game_dir,		GUI::OnMenuGameDir)
	EVT_MENU(MenuBarID::ID::save_state,		GUI::OnMenuSaveState)
	EVT_MENU(MenuBarID::ID::load_state,		GUI::OnMenuLoadState)
	EVT_MENU(MenuBarID::ID::quit,			GUI::OnMenuQuit)
	EVT_MENU(MenuBarID::ID::pause_play,		GUI::OnMenuPausePlay)
	EVT_MENU(MenuBarID::ID::reset,			GUI::OnMenuReset)
	EVT_MENU(MenuBarID::ID::stop,			GUI::OnMenuStop)
	EVT_MENU(MenuBarID::ID::size_5x,		GUI::OnMenuSize)
	EVT_MENU(MenuBarID::ID::size_10x,		GUI::OnMenuSize)
	EVT_MENU(MenuBarID::ID::size_15x,		GUI::OnMenuSize)
	EVT_MENU(MenuBarID::ID::size_20x,		GUI::OnMenuSize)
	EVT_MENU(MenuBarID::ID::size_30x,		GUI::OnMenuSize)
	EVT_MENU(MenuBarID::ID::size_40x,		GUI::OnMenuSize)
	EVT_MENU(MenuBarID::ID::size_custom,	GUI::OnMenuSize)
	EVT_MENU(MenuBarID::ID::size_fullscreen,GUI::OnMenuSize)
	EVT_MENU(MenuBarID::ID::speed_250,		GUI::OnMenuSpeed)
	EVT_MENU(MenuBarID::ID::speed_500,		GUI::OnMenuSpeed)
	EVT_MENU(MenuBarID::ID::speed_750,		GUI::OnMenuSpeed)
	EVT_MENU(MenuBarID::ID::speed_1000,		GUI::OnMenuSpeed)
	EVT_MENU(MenuBarID::ID::speed_1250,		GUI::OnMenuSpeed)
	EVT_MENU(MenuBarID::ID::speed_1500,		GUI::OnMenuSpeed)
	EVT_MENU(MenuBarID::ID::speed_custom,	GUI::OnMenuSpeed)
	EVT_MENU(MenuBarID::ID::colour_off,		GUI::OnMenuColour)
	EVT_MENU(MenuBarID::ID::colour_on,		GUI::OnMenuColour)
	EVT_MENU(MenuBarID::ID::sound_off,		GUI::OnMenuSound)
	EVT_MENU(MenuBarID::ID::sound_1,		GUI::OnMenuSound)
	EVT_MENU(MenuBarID::ID::sound_2,		GUI::OnMenuSound)
	EVT_MENU(MenuBarID::ID::sound_3,		GUI::OnMenuSound)
	EVT_MENU(MenuBarID::ID::reset_settings,	GUI::OnMenuResetSettings)
	EVT_MENU(MenuBarID::ID::github_link,	GUI::OnMenuGitHubLink)
	EVT_LISTBOX_DCLICK(listBoxID,			GUI::OnListBoxGameSelection)
	EVT_SIZE(								GUI::OnWindowSizeChanged)
	EVT_CLOSE(								GUI::OnClose)
wxEND_EVENT_TABLE()


GUI::GUI() : wxFrame(nullptr, frameID, "CHIP-8 Emulator", wxDefaultPosition, wxDefaultSize)
{
	SetClientSize(DEFAULT_WINDOW_SIZE);
	CreateMenuBar();
	DEFAULT_CLIENT_SIZE = GetClientSize(); // what remains outside of the menubar
	gameListBox = new wxListBox(this, listBoxID, wxPoint(0, 0), DEFAULT_CLIENT_SIZE);
	SDL_WindowPanel = new wxPanel(this, SDLWindowID, wxPoint(0, 0), DEFAULT_CLIENT_SIZE);

	if (config.ConfigFileExists())
		config.Load();
	else
		config.SetDefaults();
	ApplyEmulatorSettings();
	ApplyGUISettings();

	SDL_WindowPanel->Hide();
}


GUI::~GUI()
{
	delete gameListBox;
	delete SDL_WindowPanel;
}


void GUI::LaunchGame()
{
	if (!wxFileExists(activeRomPath))
	{
		wxMessageBox(wxString::Format("Error opening game; file \"%s\" does not exist.", activeRomPath));
		return;
	}

	if (!SDLInitialized && !InitializeSDL()) // InitializeSDL() returns false if unsuccessful
		return;

	if (!emulator.initialized)
		emulator.Initialize(SDL_window);

	// switch view from game list to game window
	gameListBox->Hide();
	SDL_WindowPanel->Show();
	SetClientSize(wxSize(64 * config.scale, 32 * config.scale));
	SDL_WindowPanel->SetSize(wxSize(64 * config.scale, 32 * config.scale));
	UpdateWindowLabel(true);

	emulator.StartGame(activeRomPath);
}


void GUI::QuitGame()
{
	emulator.Stop();

	// switch view from game window to game list
	SDL_WindowPanel->Hide();
	gameListBox->Show();
	SetClientSize(DEFAULT_CLIENT_SIZE);
	UpdateWindowLabel(false);
	menuEmulation->SetLabel(MenuBarID::ID::pause_play, wxT("&Pause"));
}


// returns true if successful
bool GUI::InitializeSDL()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) == -1)
	{
		wxMessageBox("Could not initialise SDL!");
		return false;
	}

	// create an "SDL window" on the wxpanel SDL_WindowPanel
	SDL_window = SDL_CreateWindowFrom((void*)SDL_WindowPanel->GetHandle());
	if (SDL_window == NULL)
	{
		wxMessageBox("Could not create the SDL window!");
		return false;
	}

	return SDLInitialized = true;
}


void GUI::CreateMenuBar()
{
	menuBar->Append(menuFile, wxT("&File"));
	menuBar->Append(menuEmulation, wxT("&Emulation"));
	menuBar->Append(menuSettings, wxT("&Settings"));
	menuBar->Append(menuInfo, wxT("&Info"));

	menuFile->Append(MenuBarID::ID::open_rom, wxT("&Open rom"));
	menuFile->Append(MenuBarID::ID::game_dir, wxT("&Set game directory"));
	menuFile->AppendSeparator();
	menuFile->Append(MenuBarID::ID::save_state, wxT("&Save state"));
	menuFile->Append(MenuBarID::ID::load_state, wxT("&Load state"));
	menuFile->AppendSeparator();
	menuFile->Append(MenuBarID::ID::quit, wxT("&Quit"));

	menuEmulation->Append(MenuBarID::ID::pause_play, wxT("&Pause"));
	menuEmulation->Append(MenuBarID::ID::reset, wxT("&Reset"));
	menuEmulation->Append(MenuBarID::ID::stop, wxT("&Stop"));

	menuSize->AppendRadioItem(MenuBarID::ID::size_5x, wxT("&320x160 (5x)"));
	menuSize->AppendRadioItem(MenuBarID::ID::size_10x, wxT("&640x320 (10x)"));
	menuSize->AppendRadioItem(MenuBarID::ID::size_15x, wxT("&960x480 (15x)"));
	menuSize->AppendRadioItem(MenuBarID::ID::size_20x, wxT("&1280x640 (20x)"));
	menuSize->AppendRadioItem(MenuBarID::ID::size_30x, wxT("&1920x960 (30x)"));
	menuSize->AppendRadioItem(MenuBarID::ID::size_40x, wxT("&2560x1280 (40x)"));
	menuSize->AppendRadioItem(MenuBarID::ID::size_custom, wxT("&Custom"));
	menuSize->AppendSeparator();
	menuSize->Append(MenuBarID::ID::size_fullscreen, wxT("&Toggle fullscreen"));
	menuSettings->AppendSubMenu(menuSize, wxT("&Window size"));

	menuSpeed->AppendRadioItem(MenuBarID::ID::speed_250, wxT("&250 CPS"));
	menuSpeed->AppendRadioItem(MenuBarID::ID::speed_500, wxT("&500 CPS"));
	menuSpeed->AppendRadioItem(MenuBarID::ID::speed_750, wxT("&750 CPS"));
	menuSpeed->AppendRadioItem(MenuBarID::ID::speed_1000, wxT("&1000 CPS"));
	menuSpeed->AppendRadioItem(MenuBarID::ID::speed_1250, wxT("&1250 CPS"));
	menuSpeed->AppendRadioItem(MenuBarID::ID::speed_1500, wxT("&1500 CPS"));
	menuSpeed->AppendRadioItem(MenuBarID::ID::speed_custom, wxT("&Custom"));
	menuSettings->AppendSubMenu(menuSpeed, wxT("&Speed"));

	menuColour->Append(MenuBarID::ID::colour_off, wxT("&Set colour (off)"));
	menuColour->Append(MenuBarID::ID::colour_on, wxT("&Set colour (on)"));
	menuSettings->AppendSubMenu(menuColour, wxT("&Colour scheme"));

	menuSound->AppendRadioItem(MenuBarID::ID::sound_off, wxT("&Sound off"));
	menuSound->AppendRadioItem(MenuBarID::ID::sound_1, wxT("&Sound effect 1"));
	menuSound->AppendRadioItem(MenuBarID::ID::sound_2, wxT("&Sound effect 2"));
	menuSound->AppendRadioItem(MenuBarID::ID::sound_3, wxT("&Sound effect 3"));
	menuSettings->AppendSubMenu(menuSound, wxT("&Sound effect"));

	menuSettings->AppendSeparator();
	menuSettings->Append(MenuBarID::ID::reset_settings, wxT("&Reset settings"));

	menuInfo->Append(MenuBarID::ID::github_link, wxT("&GitHub link"));

	SetMenuBar(menuBar);
}


// fill the game list with the games in specified rom directory
void GUI::SetupGameList()
{
	romFolderGameNames.Clear();
	romFolderGamePaths.Clear();
	gameListBox->Clear();

	wxDir::GetAllFiles(romFolderPath, &romFolderGamePaths, wxEmptyString, wxDIR_FILES);
	size_t size = romFolderGamePaths.GetCount();
	if (size == 0)
	{
		gameListBox->InsertItems(1, &EMPTY_LISTBOX_MESSAGE, 0);
		return;
	}

	for (int i = 0; i < size; i++)
		romFolderGameNames.Add(wxFileName(romFolderGamePaths[i]).GetFullName());
	gameListBox->InsertItems(romFolderGameNames, 0);
}


void GUI::ApplyEmulatorSettings()
{
	romFolderPath = config.romDir;
	emulator.SetSpeed(config.speed);
	emulator.SetWindowSize(config.scale * 64, config.scale * 32);
	emulator.SetColour(config.colourOff, false);
	emulator.SetColour(config.colourOn, true);
	emulator.SetSoundEffect(config.soundEffect);
}


void GUI::ApplyGUISettings()
{
	SetupGameList();

	if (MenuBarID::isSizeID(config.scale))
		menuSize->Check(config.scale, true); // config.scale is also equal the ID of the menu item
	else
	{
		menuSize->Check(MenuBarID::ID::size_custom, true);
		menuSize->SetLabel(MenuBarID::ID::size_custom, wxString::Format("Custom (%ix)", config.scale));
	}

	if (MenuBarID::isSpeedID(config.speed))
		menuSpeed->Check(config.speed, true); // config.speed is also equal the ID of the menu item
	else
	{
		menuSpeed->Check(MenuBarID::ID::speed_custom, true);
		menuSpeed->SetLabel(MenuBarID::ID::speed_custom, wxString::Format("Custom (%i CPS)", config.speed));
	}

	switch (config.soundEffect)
	{
	case 0:
		menuSound->Check(MenuBarID::ID::sound_off, true); break;
	case 1:
		menuSound->Check(MenuBarID::ID::sound_1, true); break;
	case 2:
		menuSound->Check(MenuBarID::ID::sound_2, true); break;
	case 3:
		menuSound->Check(MenuBarID::ID::sound_3, true); break;
	}
}


void GUI::Quit()
{
	QuitGame();
	SDL_Quit();
	Close();
}


void GUI::UpdateWindowLabel(bool gameIsRunning)
{
	wxSize size = GetClientSize();
	int scale = std::min(size.GetWidth() / 64, size.GetHeight() / 32);

	if (gameIsRunning)
		SetLabel(wxString::Format("CHIP-8 Emulator | %s | %ix%i | %i CPS", 
			wxFileName(activeRomPath).GetName(), 64 * scale, 32 * scale, emulator.cyclesPerSecond));
	else
		SetLabel("CHIP-8 Emulator");
}


void GUI::OnMenuOpen(wxCommandEvent& event)
{
	// Creates an "open file" dialog
	wxFileDialog* fileDialog = new wxFileDialog(
		this, "Choose a file to open", wxEmptyString, 
		wxEmptyString, wxFileSelectorDefaultWildcardStr,
		wxFD_OPEN | wxFD_FILE_MUST_EXIST, wxDefaultPosition);

	int buttonPressed = fileDialog->ShowModal(); // show the dialog
	wxString selectedPath = fileDialog->GetPath();
	fileDialog->Destroy();

	// if the user clicks "Open" instead of "Cancel"
	if (buttonPressed == wxID_OK)
	{
		activeRomPath = selectedPath;
		LaunchGame();
	}
}


void GUI::OnMenuGameDir(wxCommandEvent& event)
{
	ChooseGameDirDialog();
}


void GUI::ChooseGameDirDialog()
{
	// Creates a "select directory" dialog
	wxDirDialog* dirDialog = new wxDirDialog(this, "Choose a directory",
		wxEmptyString, wxDD_DIR_MUST_EXIST, wxDefaultPosition);

	int buttonPressed = dirDialog->ShowModal();
	wxString selectedPath = dirDialog->GetPath();
	dirDialog->Destroy();

	if (buttonPressed == wxID_OK)
	{
		// set the chosen directory
		romFolderPath = selectedPath;
		SetupGameList();
		config.romDir = romFolderPath;
		config.Save();
	}
}


void GUI::OnMenuSaveState(wxCommandEvent& event)
{
	if (emulator.isRunning)
		emulator.SaveState();
	else
		wxMessageBox("No game is loaded. Cannot save state.");
}


void GUI::OnMenuLoadState(wxCommandEvent& event)
{
	if (emulator.isRunning)
		emulator.LoadState();
	else
		wxMessageBox("No game is loaded. Cannot load a state.");
}


void GUI::OnMenuQuit(wxCommandEvent& event)
{
	Quit();
}


void GUI::OnMenuPausePlay(wxCommandEvent& event)
{
	if (emulator.isPaused)
	{
		menuEmulation->SetLabel(MenuBarID::ID::pause_play, wxT("&Pause"));
		emulator.Resume();
	}
	else if (emulator.isRunning)
	{
		menuEmulation->SetLabel(MenuBarID::ID::pause_play, wxT("&Resume"));
		emulator.Pause();
	}
}


void GUI::OnMenuReset(wxCommandEvent& event)
{
	if (emulator.isRunning)
	{
		menuEmulation->SetLabel(MenuBarID::ID::pause_play, wxT("&Pause"));
		emulator.Reset();
	}
}


void GUI::OnMenuStop(wxCommandEvent& event)
{
	QuitGame();
}


void GUI::OnMenuSize(wxCommandEvent& event)
{
	int id = event.GetId();

	int scale = 1;
	if (MenuBarID::isSizeID(id)) // id corr. to a predefined size
	{
		scale = id;
		if (menuSize->IsEnabled(MenuBarID::ID::size_custom))
			menuSize->SetLabel(MenuBarID::ID::size_custom, "Custom");
	}
	else if (id == MenuBarID::ID::size_custom)
	{
		wxTextEntryDialog* textEntryDialog = new wxTextEntryDialog(this,
			"The resolution will be 32s x 16s, where s is the scale.", "Enter a scale (positive integer).",
			wxEmptyString, wxTextEntryDialogStyle, wxDefaultPosition);
		textEntryDialog->SetTextValidator(wxFILTER_DIGITS);

		int buttonPressed = textEntryDialog->ShowModal();
		wxString input = textEntryDialog->GetValue();
		textEntryDialog->Destroy();

		if (buttonPressed == wxID_CANCEL)
			return;

		scale = wxAtoi(input); // string -> int
		if (scale <= 0) // input converted to int
		{
			wxMessageBox("Please enter a valid scale value.");
			return;
		}
		menuSize->SetLabel(MenuBarID::ID::size_custom, "Custom (" + input + "x)");

	}
	else if (id == MenuBarID::ID::size_fullscreen)
	{
		if (this->IsMaximized())
			this->Restore();
		else
			this->Maximize(true);
		return;
	}

	config.scale = scale;
	config.Save();

	if (emulator.isRunning)
		SetGameWindowSize(true, 64 * scale, 32 * scale);
}


void GUI::OnMenuSpeed(wxCommandEvent& event)
{
	int id = event.GetId();

	int speed = 1000;
	if (MenuBarID::isSpeedID(id))
	{
		speed = id; // the selected speed is equal to the id
		if (menuSpeed->IsEnabled(MenuBarID::ID::speed_custom))
			menuSpeed->SetLabel(MenuBarID::ID::speed_custom, "Custom");
	}
	else // custom speed
	{
		wxTextEntryDialog* textEntryDialog = new wxTextEntryDialog(this,
			"This will be the number of CPU cycles per second (CPS).", "Enter a speed (positive integer).",
			wxEmptyString, wxTextEntryDialogStyle, wxDefaultPosition);
		textEntryDialog->SetTextValidator(wxFILTER_DIGITS);

		int buttonPressed = textEntryDialog->ShowModal();
		wxString input = textEntryDialog->GetValue();
		textEntryDialog->Destroy();

		if (buttonPressed == wxID_CANCEL)
			return;

		speed = wxAtoi(input); // string -> int
		if (speed <= 0)
		{
			wxMessageBox("Please enter a valid speed value.");
			return;
		}
		menuSpeed->SetLabel(MenuBarID::ID::speed_custom, "Custom (" + input + " CPS)");
	}

	config.speed = speed;
	config.Save();
	emulator.SetSpeed(speed);
	UpdateWindowLabel(emulator.isRunning);
}


void GUI::OnMenuColour(wxCommandEvent& event)
{
	wxColourDialog* colourDialog = new wxColourDialog(this, NULL);
	int buttonPressed = colourDialog->ShowModal();
	wxColourData colourData = colourDialog->GetColourData();
	colourDialog->Destroy();

	if (buttonPressed == wxID_CANCEL)
		return;

	wxColour colour = colourData.GetColour();
	if (event.GetId() == MenuBarID::ID::colour_off)
	{
		emulator.SetColour(colour, false);
		config.colourOff = colour;
	}
	else
	{
		emulator.SetColour(colour, true);
		config.colourOn = colour;
	}
	config.Save();
}


void GUI::OnMenuSound(wxCommandEvent& event)
{
	switch (event.GetId())
	{
	case MenuBarID::ID::sound_off:
		config.soundEffect = 0; break;
	case MenuBarID::ID::sound_1:
		config.soundEffect = 1; break;
	case MenuBarID::ID::sound_2:
		config.soundEffect = 2; break;
	case MenuBarID::ID::sound_3:
		config.soundEffect = 3; break;
	}

	config.Save();
	emulator.SetSoundEffect(config.soundEffect);
}


void GUI::OnMenuResetSettings(wxCommandEvent& event)
{
	config.SetDefaults();
	ApplyEmulatorSettings();
	ApplyGUISettings();
	if (emulator.isRunning)
		SetGameWindowSize(true, config.scale * 64, config.scale * 32);
}


void GUI::OnMenuGitHubLink(wxCommandEvent& event)
{
	wxLaunchDefaultBrowser("https://github.com/TesseracT33/CHIP-8-Emulator", wxBROWSER_NEW_WINDOW);
}


void GUI::OnListBoxGameSelection(wxCommandEvent& event)
{
	if (gameListBox->IsEmpty())
	{
		ChooseGameDirDialog();
		return;
	}

	// from the rom name (name.ext), get the full path of the file from array `romFolderGamePaths'
	wxString romName = event.GetString();
	bool romFound = false;
	for (int i = 0; i < romFolderGamePaths.GetCount(); i++)
		if (wxFileNameFromPath(romFolderGamePaths[i]).IsSameAs(romName))
		{
			activeRomPath = romFolderGamePaths[i];
			romFound = true;
		}

	if (romFound)
		LaunchGame();
	else
		wxMessageBox(wxString::Format("Error opening game; could not locate file \"%s\".", romName));
}


void GUI::OnWindowSizeChanged(wxSizeEvent& event)
{
	wxSize size = GetSize();

	// if a game is running, we need to set the size of the SDL window to the main window client size specifically
	if (emulator.isRunning)
	{
		wxSize size = GetClientSize();
		SetGameWindowSize(false, size.GetWidth(), size.GetHeight());
	}
}


void GUI::OnClose(wxCloseEvent& event)
{
	QuitGame();
	SDL_Quit();
	event.Skip();
}


// setFrameSize indicates whether or not we should change the main window client size
// this is needed, because we may want to change the size of only the game window, and not the main window
void GUI::SetGameWindowSize(bool setFrameSize, int width, int height)
{
	if (setFrameSize)
		SetClientSize(wxSize(width, height));

	SDL_WindowPanel->SetSize(wxSize(width, height));
	emulator.SetWindowSize(width, height);
	UpdateWindowLabel(true);
}