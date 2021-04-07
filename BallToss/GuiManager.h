#pragma once
#include <irrlicht.h>

class GuiManager
{
public:

	GuiManager();
	
	// Method for input handling
	void handleInput(const irr::SEvent& event);

	void createGuiElements();
	
	void updateTimerGui(float seconds) const;
	void changeScore(int score) const;
	
	void moveSlider();
	float getSliderValue() const { return sliderValue; }
	void setSliderVisible(bool visible) const;
	void resetSlider() { sliderValue = 0.f; }
	
	void showUnderWater(const bool show) const { underwater->setVisible(show); }
	
	void showMainMenu() const;
	void hideMainMenu() const;
	
	void showPauseMenu() const;
	void hidePauseMenu() const;
	
	void showEndGameWindow(irr::core::stringw& messageText) const;
	void hideEndGameWindow() const;
	
	void showOtherGuiElements() const;
	void hideOtherGuiElements() const;

	void showLoadingText() const;
	void hideLoadingText() const;
	
	void setWindowWidth(const irr::u32 width) { windowDimensions.Width = width; }
	void setWindowHeight(const irr::u32 height) { windowDimensions.Height = height; }

	irr::core::stringw getPlayerName() const { return playerName; }
	void setPlayerName(const irr::core::stringw& playerName) { this->playerName = playerName; }
	
private:
	irr::core::dimension2d<irr::u32> windowDimensions;

	irr::core::stringw playerName;

	irr::gui::IGUIStaticText* playerNameText;
	irr::gui::IGUIStaticText* loadingText;
	irr::gui::IGUIStaticText* timerText;
	irr::gui::IGUIStaticText* scoreText;
	irr::gui::IGUIStaticText* endGameText;
	
	irr::gui::IGUIWindow* mainMenuWindow;
	irr::gui::IGUIWindow* pauseMenuWindow;
	irr::gui::IGUIWindow* optionsMenuWindow;
	irr::gui::IGUIWindow* highScoreWindow;
	irr::gui::IGUIWindow* endGameWindow;

	irr::gui::IGUIListBox* scoreListBox;
	
	irr::gui::IGUIImage* mainMenuBackground;
	irr::gui::IGUIImage* menuBackground;
	irr::gui::IGUIImage* crossHair;
	irr::gui::IGUIImage* slider;
	irr::gui::IGUIImage* sliderMask;
	irr::gui::IGUIImage* underwater;
	
	float sliderValue;
	bool sliderChange;

	float musicSliderValue;
	float soundSliderValue;
	
	// Functions
	void createMainMenu();
	void createPauseMenu();
	void createOptionsMenu();
	void createHighScoreMenu();
	void createEndGameWindow();
	void showOptionsMenu() const;
	void hideOptionsMenu() const;
	void showHighScoreMenu() const;
	void hideHighScoreMenu() const;
	void readHighScore();
	
    // Values used to identify individual GUI elements
    enum
    {
        GUI_ID_BUTTON_START,
        GUI_ID_BUTTON_OPTIONS,
    	GUI_ID_BUTTON_HIGHSCORE,
        GUI_ID_BUTTON_QUIT,
    	GUI_ID_BUTTON_RESUME,
		GUI_ID_BUTTON_RESUME_QUIT,
    	GUI_ID_BUTTON_OPTIONS_CLOSE,
		GUI_ID_BUTTON_HIGHSCORE_CLOSE,
		GUI_ID_BUTTON_END_CLOSE,

		GUI_ID_PLAYER_NAME,
    	
    	GUI_ID_CHECKBOX_MUSIC,
    	GUI_ID_CHECKBOX_SOUND,

    	GUI_ID_MUSIC_VOLUME,
    	GUI_ID_SOUND_VOLUME
    };
};