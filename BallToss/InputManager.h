#pragma once
#include <irrlicht.h>

class InputManager
{
public:

	InputManager();
	~InputManager();

	// Method for input handling
	void handleInput(const irr::SEvent& event);
	
	// Method to check if certain key is down
	bool isKeyDown(irr::EKEY_CODE keyCode);
	
	// Used to reset state of key
	void resetCode(irr::EKEY_CODE keyCode);

private:
	// Array to store state of each key
	bool keys[irr::KEY_KEY_CODES_COUNT];
};