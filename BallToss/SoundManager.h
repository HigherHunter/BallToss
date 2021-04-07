#pragma once
#include <irrKlang.h>

class SoundManager
{
public:
	static SoundManager* getInstance();

	void playMenuMusic() const;
	void stopMenuMusic() const;
	void playBackgroundMusic() const;
	void stopBackgroundMusic() const;

	void playScoreSound() const;
	
	void playShootBallSLow() const;
	void playShootBallMed() const;
	void playShootBallFast() const;
	void playShootUnderWater() const;
	
	void playUISound() const;

	void playEndGame() const;
	
	void setMusicVolume(float volume);
	void setSoundVolume(float volume);

private:
	static SoundManager* instance;
	SoundManager();

	irrklang::ISoundEngine* soundEngine;

	irrklang::ISound* menuMusic;
	irrklang::ISound* background;
	
	float musicVolume, soundFXVolume;
};