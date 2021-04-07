#include "SoundManager.h"
using namespace irrklang;

SoundManager* SoundManager::instance = nullptr;

SoundManager::SoundManager()
{
	soundEngine = createIrrKlangDevice();

	if (!soundEngine)
	{
	}
	
	musicVolume = 1.f;
	soundFXVolume = 1.f;

	menuMusic = soundEngine->play2D("../media/Audio/Music/menu.ogg", true, false, true);
	background = soundEngine->play2D("../media/Audio/Music/background.ogg", true, false, true);
	
	menuMusic->setIsPaused(true);
	background->setIsPaused(true);
}

SoundManager* SoundManager::getInstance()
{
	if (!instance)
		instance = new SoundManager();

	return instance;
}

void SoundManager::playMenuMusic() const
{
	menuMusic->setIsPaused(false);
}

void SoundManager::stopMenuMusic() const
{
	menuMusic->setIsPaused(true);
}

void SoundManager::playBackgroundMusic() const
{
	background->setIsPaused(false);
}

void SoundManager::stopBackgroundMusic() const
{
	background->setIsPaused(true);
}

void SoundManager::playScoreSound() const
{
	if (soundFXVolume > 0)
		soundEngine->play2D("../media/Audio/Sounds/score.ogg", false, false, true)->setVolume(soundFXVolume);
}

void SoundManager::playShootBallSLow() const
{
	if (soundFXVolume > 0)
		soundEngine->play2D("../media/Audio/Sounds/shootBallSlow.ogg", false, false, true)->setVolume(soundFXVolume);
}

void SoundManager::playShootBallMed() const
{
	if (soundFXVolume > 0)
		soundEngine->play2D("../media/Audio/Sounds/shootBallMed.ogg", false, false, true)->setVolume(soundFXVolume);
}

void SoundManager::playShootBallFast() const
{
	if (soundFXVolume > 0)
		soundEngine->play2D("../media/Audio/Sounds/shootBallFast.ogg", false, false, true)->setVolume(soundFXVolume);
}

void SoundManager::playShootUnderWater() const
{
	if (soundFXVolume > 0)
		soundEngine->play2D("../media/Audio/Sounds/shootSubmergedBall.ogg", false, false, true)->setVolume(soundFXVolume);
}

void SoundManager::playUISound() const
{
	if (soundFXVolume > 0)
		soundEngine->play2D("../media/Audio/Sounds/button.ogg", false, false, true)->setVolume(soundFXVolume);
}

void SoundManager::playEndGame() const
{
	if (soundFXVolume > 0)
		soundEngine->play2D("../media/Audio/Sounds/endRound.ogg", false, false, true)->setVolume(soundFXVolume);
}

void SoundManager::setMusicVolume(const float volume)
{
	musicVolume = volume * 0.01f;
	menuMusic->setVolume(musicVolume);
	background->setVolume(musicVolume);
}

void SoundManager::setSoundVolume(const float volume)
{
	soundFXVolume = volume * 0.01f;
}