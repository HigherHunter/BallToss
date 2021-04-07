#pragma once
#include <irrlicht.h>

class SaveManager
{
public:
	SaveManager();

	bool readHighScore();
	bool writeHighScore() const;

	irr::core::map<irr::core::stringw, irr::core::stringw>& getHighScoreMap() { return highScoreMap; }
	
	void setHighScore(const irr::core::stringw& name, const irr::core::stringw& value);
	void setHighScore(const irr::core::stringw& name, irr::s32 value);

private:	
	irr::core::map<irr::core::stringw, irr::core::stringw> highScoreMap;
};