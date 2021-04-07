#include "GuiManager.h"
#include "GameManager.h"
#include "InputManager.h"
#include "SoundManager.h"
#include "SaveManager.h"

using namespace irr;
using namespace core;
using namespace video;
using namespace scene;
using namespace gui;

extern IrrlichtDevice* device;
extern GameManager* gameManager;
extern InputManager* inputManager;
extern SaveManager* saveManager;

GuiManager::GuiManager()
{
	windowDimensions.Width = 1280;
	windowDimensions.Height = 720;

	timerText = nullptr;
	scoreText = nullptr;
	endGameText = nullptr;

	mainMenuWindow = nullptr;
	pauseMenuWindow = nullptr;
	optionsMenuWindow = nullptr;
	endGameWindow = nullptr;
	
	mainMenuBackground = nullptr;
	menuBackground = nullptr;
	crossHair = nullptr;
	slider = nullptr;
	sliderMask = nullptr;
	underwater = nullptr;
	
	sliderValue = 0.f;
	sliderChange = false;

	musicSliderValue = 100.f;
	soundSliderValue = 100.f;
}

void GuiManager::handleInput(const SEvent& event)
{
	if (event.EventType == EET_GUI_EVENT)
	{
		const s32 id = event.GUIEvent.Caller->getID();

		switch (event.GUIEvent.EventType)
		{
		case EGET_BUTTON_CLICKED:

			switch (id)
			{
			case GUI_ID_BUTTON_START:
			{
				SoundManager::getInstance()->playUISound();
				playerName = device->getGUIEnvironment()->getRootGUIElement()->getElementFromId(GUI_ID_PLAYER_NAME, true)->getText();
				playerNameText->setText(playerName.c_str());
					if (gameManager->getIsWorldCreated())
					{
						hideMainMenu();
						showLoadingText();
						mainMenuBackground->setVisible(true);
						device->getVideoDriver()->beginScene();
						device->getGUIEnvironment()->drawAll();
						device->getVideoDriver()->endScene();
						gameManager->restartGame();
						mainMenuBackground->setVisible(false);
					}
					else
					{
						hideMainMenu();
						showLoadingText();
						mainMenuBackground->setVisible(true);
						device->getVideoDriver()->beginScene();
						device->getGUIEnvironment()->drawAll();
						device->getVideoDriver()->endScene();
						gameManager->createScene();
						mainMenuBackground->setVisible(false);
					}
				showOtherGuiElements();
				gameManager->setMenuActiveState(false);
				inputManager->resetCode(KEY_LBUTTON);
			}
				break;
			case GUI_ID_BUTTON_RESUME:
			{
				hidePauseMenu();
				showOtherGuiElements();
				gameManager->setMenuActiveState(false);
				SoundManager::getInstance()->playUISound();
				inputManager->resetCode(KEY_LBUTTON);
			}
				break;
			case GUI_ID_BUTTON_OPTIONS:
				hideMainMenu();
				showOptionsMenu();
				SoundManager::getInstance()->playUISound();
				break;
			case GUI_ID_BUTTON_HIGHSCORE:
				hideMainMenu();
				saveManager->readHighScore();
				readHighScore();
				showHighScoreMenu();
				SoundManager::getInstance()->playUISound();
				break;
			case GUI_ID_BUTTON_QUIT:
				SoundManager::getInstance()->playUISound();
				device->closeDevice();
				break;
			case GUI_ID_BUTTON_RESUME_QUIT:
				hidePauseMenu();
				showMainMenu();
				SoundManager::getInstance()->playUISound();
				SoundManager::getInstance()->stopBackgroundMusic();
				SoundManager::getInstance()->playMenuMusic();
				break;
			case GUI_ID_BUTTON_OPTIONS_CLOSE:
				hideOptionsMenu();
				showMainMenu();
				SoundManager::getInstance()->playUISound();
				break;
			case GUI_ID_BUTTON_HIGHSCORE_CLOSE:
				hideHighScoreMenu();
				showMainMenu();
				SoundManager::getInstance()->playUISound();
				break;
			case GUI_ID_BUTTON_END_CLOSE:
			{
				gameManager->restartGame();
				hideEndGameWindow();
				showOtherGuiElements();
				gameManager->setMenuActiveState(false);
				SoundManager::getInstance()->playUISound();
				inputManager->resetCode(KEY_LBUTTON);
			}
				break;
			default:
				break;
			}

			break;
		case EGET_SCROLL_BAR_CHANGED:
			if (id == GUI_ID_MUSIC_VOLUME)
			{
				musicSliderValue = ((IGUIScrollBar*)event.GUIEvent.Caller)->getPos();
				SoundManager::getInstance()->setMusicVolume(musicSliderValue);
			}
			else if (id == GUI_ID_SOUND_VOLUME)
			{
				soundSliderValue = ((IGUIScrollBar*)event.GUIEvent.Caller)->getPos();
				SoundManager::getInstance()->setSoundVolume(soundSliderValue);
			}
			break;
		case EGET_CHECKBOX_CHANGED:
			if (id == GUI_ID_CHECKBOX_MUSIC)
			{
				if (((IGUICheckBox*)event.GUIEvent.Caller)->isChecked())
				{
					SoundManager::getInstance()->setMusicVolume(musicSliderValue);
				}
				else if (!((IGUICheckBox*)event.GUIEvent.Caller)->isChecked())
				{
					SoundManager::getInstance()->setMusicVolume(0);
				}
			}
			else if (id == GUI_ID_CHECKBOX_SOUND)
			{
				if (((IGUICheckBox*)event.GUIEvent.Caller)->isChecked())
				{
					SoundManager::getInstance()->setSoundVolume(soundSliderValue);
				}
				else if (!((IGUICheckBox*)event.GUIEvent.Caller)->isChecked())
				{
					SoundManager::getInstance()->setSoundVolume(0);
				}
			}
			break;
		default:
			break;
		}
	}
}

void GuiManager::createGuiElements()
{
	// Text for timer in bottom right corner
	timerText = device->getGUIEnvironment()->addStaticText(L"", rect<s32>(windowDimensions.Width - 60, windowDimensions.Height - 50, windowDimensions.Width, windowDimensions.Height), true, false, nullptr, -1, true);
	timerText->setVisible(false);

	// Text for score in middle top
	scoreText = device->getGUIEnvironment()->addStaticText(L"0", rect<s32>(windowDimensions.Width * 0.5f - 100, 0, windowDimensions.Width * 0.5 + 100, 100), false, false);
	scoreText->setTextAlignment(EGUIA_CENTER, EGUIA_CENTER);
	
	scoreText->setOverrideFont(device->getGUIEnvironment()->getFont("../media/fonts/fontCalibri48/myfont.xml"));
	scoreText->setVisible(false);

	// Text for player name in top left
	playerNameText = device->getGUIEnvironment()->addStaticText(L"0", rect<s32>(10, 0, 150, 40), false, false);
	playerNameText->setVisible(false);
	
	// CrossHair in center of the screen
	crossHair = device->getGUIEnvironment()->addImage(device->getVideoDriver()->getTexture("../media/textures/crossHair.png"), vector2d<int>(windowDimensions.Width * 0.5f - 32, windowDimensions.Height * 0.5f - 32), true);
	crossHair->setVisible(false);

	// Power slider
	slider = device->getGUIEnvironment()->addImage(device->getVideoDriver()->getTexture("../media/textures/bar.png"), vector2d<int>(windowDimensions.Width * 0.5f - 64, windowDimensions.Height * 0.5f + 64), true);
	slider->setVisible(false);

	// Power slider dark mask
	sliderMask = device->getGUIEnvironment()->addImage(device->getVideoDriver()->getTexture("../media/textures/bar.png"), vector2d<int>(windowDimensions.Width * 0.5f - 64, windowDimensions.Height * 0.5f + 64), true);
	sliderMask->setColor(SColor(100, 0, 0, 0));
	sliderMask->setVisible(false);

	// Underwater screen cover image
	underwater = device->getGUIEnvironment()->addImage(device->getVideoDriver()->getTexture("../media/textures/underwater.png"), vector2d<int>(0, 0), true);
	underwater->setColor(SColor(200, 52, 74, 77));
	underwater->setVisible(false);

	createMainMenu();

	// Text for loading in center of the screen
	loadingText = device->getGUIEnvironment()->addStaticText(L"Loading...", rect<s32>(windowDimensions.Width * 0.5f - 100, windowDimensions.Height * 0.5f - 50, windowDimensions.Width * 0.5 + 100, windowDimensions.Height * 0.5f + 50), false, false);
	loadingText->setTextAlignment(EGUIA_CENTER, EGUIA_CENTER);

	loadingText->setOverrideFont(device->getGUIEnvironment()->getFont("../media/fonts/fontCalibri48/myfont.xml"));
	loadingText->setVisible(false);
	
	createPauseMenu();
	createOptionsMenu();
	createHighScoreMenu();
	createEndGameWindow();
}

void GuiManager::updateTimerGui(const float seconds) const
{
	const int sec = seconds;
	const int min = sec / 60;
	
	stringw str(L"");
	str.append(stringw(min % 60));
	str += L":";
	str.append(stringw(sec % 60));
	
	timerText->setText(str.c_str());
}

void GuiManager::changeScore(const int score) const
{
	stringw str(L"");
	str.append(stringw(score));
	scoreText->setText(str.c_str());
}

void GuiManager::moveSlider()
{
	if (sliderChange)
		sliderValue -= 1.f;
	else
		sliderValue += 1.f;
	if (sliderValue >= 128)
		sliderChange = true;
	else if (sliderValue <= 0)
		sliderChange = false;
	sliderMask->setRelativePosition(rect<s32>(windowDimensions.Width * 0.5f - 64 + sliderValue, windowDimensions.Height * 0.5f + 64, windowDimensions.Width * 0.5f + 64, windowDimensions.Height * 0.5f + 64 + 16));
}

void GuiManager::setSliderVisible(const bool visible) const
{
	slider->setVisible(visible);
	sliderMask->setVisible(visible);
}

void GuiManager::showMainMenu() const
{
	mainMenuWindow->setVisible(true);
	mainMenuBackground->setVisible(true);
}

void GuiManager::hideMainMenu() const
{
	mainMenuWindow->setVisible(false);
	mainMenuBackground->setVisible(false);
}

void GuiManager::showPauseMenu() const
{
	pauseMenuWindow->setVisible(true);
	mainMenuBackground->setVisible(true);
}

void GuiManager::hidePauseMenu() const
{
	pauseMenuWindow->setVisible(false);
	mainMenuBackground->setVisible(false);
}

void GuiManager::showOptionsMenu() const
{
	optionsMenuWindow->setVisible(true);
	mainMenuBackground->setVisible(true);
}

void GuiManager::hideOptionsMenu() const
{
	optionsMenuWindow->setVisible(false);
	mainMenuBackground->setVisible(false);
}

void GuiManager::showHighScoreMenu() const
{
	highScoreWindow->setVisible(true);
	mainMenuBackground->setVisible(true);
}

void GuiManager::hideHighScoreMenu() const
{
	highScoreWindow->setVisible(false);
	mainMenuBackground->setVisible(false);
}

void GuiManager::readHighScore()
{
	scoreListBox->clear();
	map<stringw, stringw>::Iterator i = saveManager->getHighScoreMap().getIterator();
	for (; !i.atEnd(); i++)
	{
		stringw str = i->getKey().c_str();
		str += " ";
		str += i->getValue().c_str();
		scoreListBox->addItem(str.c_str());
	}
}

void GuiManager::showEndGameWindow(stringw& messageText) const
{
	endGameText->setText(messageText.c_str());
	endGameText->setTextAlignment(EGUIA_CENTER, EGUIA_CENTER);
	endGameWindow->setVisible(true);
}

void GuiManager::hideEndGameWindow() const
{
	endGameWindow->setVisible(false);
}

void GuiManager::showOtherGuiElements() const
{
	scoreText->setVisible(true);
	timerText->setVisible(true);
	crossHair->setVisible(true);
	playerNameText->setVisible(true);
}

void GuiManager::hideOtherGuiElements() const
{
	scoreText->setVisible(false);
	timerText->setVisible(false);
	crossHair->setVisible(false);
	playerNameText->setVisible(false);
}

void GuiManager::showLoadingText() const
{
	loadingText->setVisible(true);
}

void GuiManager::hideLoadingText() const
{
	loadingText->setVisible(false);
}

void GuiManager::createMainMenu()
{	
	device->getGUIEnvironment()->getSkin()->setColor(EGDC_BUTTON_TEXT, SColor(255, 43, 93, 209));
	device->getGUIEnvironment()->getSkin()->setColor(EGDC_SCROLLBAR, SColor(255, 100, 100, 100));

	// Background image on the screen
	mainMenuBackground = device->getGUIEnvironment()->addImage(device->getVideoDriver()->getTexture("../media/textures/mainMenuBackground.png"), vector2d<int>(0, 0));
	mainMenuBackground->setVisible(true);
	
	mainMenuWindow = device->getGUIEnvironment()->addWindow(core::rect<s32>(windowDimensions.Width * 0.5 - 125, windowDimensions.Height * 0.5 - 300, windowDimensions.Width * 0.5 + 125, windowDimensions.Height * 0.5 + 300),
		false, L"", nullptr, 5000);
	mainMenuWindow->setDrawTitlebar(false);
	mainMenuWindow->getCloseButton()->setVisible(false);
	mainMenuWindow->setDrawBackground(true);
	mainMenuWindow->setDraggable(false);

	// Border image for menu window
	menuBackground = device->getGUIEnvironment()->addImage(device->getVideoDriver()->getTexture("../media/textures/menuBackground.png"), vector2d<int>(0, 0), true, mainMenuWindow);
	menuBackground->setAlignment(EGUIA_SCALE, EGUIA_SCALE, EGUIA_SCALE, EGUIA_SCALE);
	menuBackground->setRelativePosition(rect<s32>(0, 0, 250, 600));
	menuBackground->setScaleImage(true);

	device->getGUIEnvironment()->addStaticText(L"Name:", core::rect<s32>(85, 50, 195, 100), false, false, mainMenuWindow);
	device->getGUIEnvironment()->addEditBox(L"Player", core::rect<s32>(55, 100, 195, 140), true, mainMenuWindow, GUI_ID_PLAYER_NAME)->setMax(7);
	device->getGUIEnvironment()->addButton(core::rect<s32>(55, 190, 195, 240), mainMenuWindow, GUI_ID_BUTTON_START, L"Start");
	device->getGUIEnvironment()->addButton(core::rect<s32>(55, 290, 195, 340), mainMenuWindow, GUI_ID_BUTTON_OPTIONS, L"Options");
	device->getGUIEnvironment()->addButton(core::rect<s32>(55, 390, 195, 440), mainMenuWindow, GUI_ID_BUTTON_HIGHSCORE, L"HighScore");
	device->getGUIEnvironment()->addButton(core::rect<s32>(55, 490, 195, 540), mainMenuWindow, GUI_ID_BUTTON_QUIT, L"Quit");

	device->getGUIEnvironment()->addStaticText(L"Main Menu",
		core::rect<s32>(55, -5, 225, 30), false, false, mainMenuWindow)->setOverrideColor(SColor(255, 255, 255, 0));
}

void GuiManager::createPauseMenu()
{
	device->getGUIEnvironment()->getSkin()->setColor(EGDC_BUTTON_TEXT, SColor(255, 43, 93, 209));

	pauseMenuWindow = device->getGUIEnvironment()->addWindow(core::rect<s32>(windowDimensions.Width * 0.5 - 100, windowDimensions.Height * 0.5 - 200, windowDimensions.Width * 0.5 + 100, windowDimensions.Height * 0.5 + 200),
		false, L"", nullptr, 5000);
	pauseMenuWindow->setDrawTitlebar(false);
	pauseMenuWindow->getCloseButton()->setVisible(false);
	pauseMenuWindow->setDrawBackground(true);
	pauseMenuWindow->setDraggable(false);

	menuBackground = device->getGUIEnvironment()->addImage(device->getVideoDriver()->getTexture("../media/textures/menuBackground.png"), vector2d<int>(0, 0), true, pauseMenuWindow);

	device->getGUIEnvironment()->addButton(core::rect<s32>(50, 100, 150, 150), pauseMenuWindow, GUI_ID_BUTTON_RESUME, L"Resume");
	device->getGUIEnvironment()->addButton(core::rect<s32>(50, 250, 150, 300), pauseMenuWindow, GUI_ID_BUTTON_RESUME_QUIT, L"Quit");

	device->getGUIEnvironment()->addStaticText(L"Pause Menu",
		core::rect<s32>(30, -5, 200, 30), false, false, pauseMenuWindow)->setOverrideColor(SColor(255, 255, 255, 0));

	pauseMenuWindow->setVisible(false);
}

void GuiManager::createOptionsMenu()
{
	device->getGUIEnvironment()->getSkin()->setColor(EGDC_BUTTON_TEXT, SColor(255, 43, 93, 209));

	optionsMenuWindow = device->getGUIEnvironment()->addWindow(core::rect<s32>(windowDimensions.Width * 0.5 - 200, windowDimensions.Height * 0.5 - 200, windowDimensions.Width * 0.5 + 200, windowDimensions.Height * 0.5 + 200),
		false, L"", nullptr, 5000);
	optionsMenuWindow->setDrawTitlebar(false);
	optionsMenuWindow->getCloseButton()->setVisible(false);
	optionsMenuWindow->setDrawBackground(true);
	optionsMenuWindow->setDraggable(false);

	device->getGUIEnvironment()->addStaticText(L"Sound Settings", core::rect<s32>(100, 0, 300, 40), false, false, optionsMenuWindow)->setOverrideColor(SColor(255, 255, 255, 0));
	
	// Add check box for music
	device->getGUIEnvironment()->addCheckBox(true, rect<s32>(10, 60, 110, 100), optionsMenuWindow, GUI_ID_CHECKBOX_MUSIC, L"Music");

	// Add check box for sound
	device->getGUIEnvironment()->addCheckBox(true, rect<s32>(130, 60, 260, 100), optionsMenuWindow, GUI_ID_CHECKBOX_SOUND, L"SoundFX");
	
	device->getGUIEnvironment()->addStaticText(L"Music volume:",
		rect<s32>(10, 120, 200, 160), false, false, optionsMenuWindow);
	
	// Add scroll bar for music volume
	IGUIScrollBar* scrollbarMusic = device->getGUIEnvironment()->addScrollBar(true, rect<s32>(10, 170, 160, 190), optionsMenuWindow, GUI_ID_MUSIC_VOLUME);
	scrollbarMusic->setMax(100);
	scrollbarMusic->setPos(100);
	
	musicSliderValue = 100.f;
	
	// Add scroll bar for music soundFX
	device->getGUIEnvironment()->addStaticText(L"SoundFX volume:",
		rect<s32>(10, 210, 230, 250), false, false, optionsMenuWindow);
	
	IGUIScrollBar* scrollbarSound = device->getGUIEnvironment()->addScrollBar(true, rect<s32>(10, 260, 160, 280), optionsMenuWindow, GUI_ID_SOUND_VOLUME);
	scrollbarSound->setMax(100);
	scrollbarSound->setPos(100);

	soundSliderValue = 100.f;
	
	device->getGUIEnvironment()->addButton(core::rect<s32>(150, 325, 250, 375), optionsMenuWindow, GUI_ID_BUTTON_OPTIONS_CLOSE, L"Close");

	optionsMenuWindow->setVisible(false);
}

void GuiManager::createHighScoreMenu()
{
	device->getGUIEnvironment()->getSkin()->setColor(EGDC_BUTTON_TEXT, SColor(255, 43, 93, 209));

	highScoreWindow = device->getGUIEnvironment()->addWindow(core::rect<s32>(windowDimensions.Width * 0.5 - 200, windowDimensions.Height * 0.5 - 250, windowDimensions.Width * 0.5 + 200, windowDimensions.Height * 0.5 + 250),
		false, L"", nullptr, 5000);
	highScoreWindow->setDrawTitlebar(false);
	highScoreWindow->getCloseButton()->setVisible(false);
	highScoreWindow->setDrawBackground(true);
	highScoreWindow->setDraggable(false);

	device->getGUIEnvironment()->addStaticText(L"High Score", core::rect<s32>(130, 0, 330, 40), false, false, highScoreWindow)->setOverrideColor(SColor(255, 255, 255, 0));
	
	scoreListBox = device->getGUIEnvironment()->addListBox(rect<s32>(50, 50, 350, 350), highScoreWindow, -1, true);
	scoreListBox->setSelected(-1);
	
	device->getGUIEnvironment()->addButton(core::rect<s32>(150, 400, 250, 450), highScoreWindow, GUI_ID_BUTTON_HIGHSCORE_CLOSE, L"Close");

	highScoreWindow->setVisible(false);
}

void GuiManager::createEndGameWindow()
{
	endGameWindow = device->getGUIEnvironment()->addWindow(core::rect<s32>(windowDimensions.Width * 0.5 - 200, windowDimensions.Height * 0.5 - 200, windowDimensions.Width * 0.5 + 200, windowDimensions.Height * 0.5 + 200),
		false, L"", nullptr, 5000);
	endGameWindow->setDrawTitlebar(false);
	endGameWindow->getCloseButton()->setVisible(false);
	endGameWindow->setDrawBackground(true);
	endGameWindow->setDraggable(false);

	device->getGUIEnvironment()->addImage(device->getVideoDriver()->getTexture("../media/textures/endGameBackground.png"), vector2d<int>(0, 0), true, endGameWindow);
	
	endGameText = device->getGUIEnvironment()->addStaticText(L"", rect<s32>(0, 0, 400, 400), true, true, endGameWindow, -1, true);
	endGameText->setTextAlignment(EGUIA_CENTER, EGUIA_CENTER);

	device->getGUIEnvironment()->addButton(core::rect<s32>(150, 325, 250, 375), endGameWindow, GUI_ID_BUTTON_END_CLOSE, L"Close");

	endGameWindow->setVisible(false);
}