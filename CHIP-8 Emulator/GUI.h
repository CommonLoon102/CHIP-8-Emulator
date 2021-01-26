#pragma once

#include "Config.h"
#include "Emulator.h"
#include "wx/wx.h"
#include <wx/filedlg.h>
#include <wx/menu.h>
#include <wx/panel.h>


class GUI : public wxFrame
{
public:
	GUI();
	~GUI();

private:
	wxDECLARE_EVENT_TABLE();

	static const int frameID     = 10000;
	static const int listBoxID   = 10001;
	static const int SDLWindowID = 10002;
	const wxString EMPTY_LISTBOX_MESSAGE = wxString("Double click this text to choose a game directory");
	const wxSize DEFAULT_WINDOW_SIZE = wxSize(500, 500);
	wxSize DEFAULT_CLIENT_SIZE; // the area that is left outside of the menubar

	bool SDLInitialized = false;
	wxString activeRomPath; // path to currently or last played rom file
	wxString romFolderPath; // selected rom directory
	wxArrayString romFolderGameNames; // array of names of all roms displayed in the list of games
	wxArrayString romFolderGamePaths; // array of paths of all roms displayed

	Config config;
	Emulator emulator;

	// as per the wxwidgets documentation, all menus should be created on the heap
	// however, there is no need to manually use delete on any of these pointers
	// https://docs.wxwidgets.org/3.0/classwx_menu.html
	wxMenuBar* menuBar = new wxMenuBar();
	wxMenu* menuFile = new wxMenu();
	wxMenu* menuEmulation = new wxMenu();
	wxMenu* menuSettings = new wxMenu();
	wxMenu* menuInfo = new wxMenu();
	wxMenu* menuSize = new wxMenu();
	wxMenu* menuSpeed = new wxMenu();
	wxMenu* menuColour = new wxMenu();
	wxMenu* menuSound = new wxMenu();

	wxListBox* gameListBox = nullptr;
	wxPanel* SDL_WindowPanel = nullptr;
	SDL_Window* SDL_window = nullptr;

	bool InitializeSDL();
	void ApplyEmulatorSettings();
	void ApplyGUISettings();
	void ChooseGameDirDialog();
	void CreateMenuBar();
	void LaunchGame();
	void Quit();
	void QuitGame();
	void SetGameWindowSize(bool setFrameSize, int width, int height);
	void SetupGameList();
	void UpdateWindowLabel(bool gameIsRunning);

	void OnMenuOpen(wxCommandEvent& event);
	void OnMenuGameDir(wxCommandEvent& event);
	void OnMenuSaveState(wxCommandEvent& event);
	void OnMenuLoadState(wxCommandEvent& event);
	void OnMenuQuit(wxCommandEvent& event);
	void OnMenuPausePlay(wxCommandEvent& event);
	void OnMenuReset(wxCommandEvent& event);
	void OnMenuStop(wxCommandEvent& event);
	void OnMenuSize(wxCommandEvent& event);
	void OnMenuSpeed(wxCommandEvent& event);
	void OnMenuColour(wxCommandEvent& event);
	void OnMenuSound(wxCommandEvent& event);
	void OnMenuResetSettings(wxCommandEvent& event);
	void OnMenuGitHubLink(wxCommandEvent& event);
	void OnListBoxGameSelection(wxCommandEvent& event);
	void OnWindowSizeChanged(wxSizeEvent& event);
	void OnClose(wxCloseEvent& event);
};