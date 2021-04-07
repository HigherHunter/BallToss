#include "InputManager.h"
using namespace irr;

InputManager::InputManager()
{
	for (u32 i = 0; i < KEY_KEY_CODES_COUNT; i++)
		keys[i] = false;
}

InputManager::~InputManager()
= default;

void InputManager::handleInput(const SEvent& event)
{
	if (event.EventType == EET_KEY_INPUT_EVENT)
		keys[event.KeyInput.Key] = event.KeyInput.PressedDown;
	else if (event.EventType == EET_MOUSE_INPUT_EVENT)
		if (event.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN)
			keys[KEY_LBUTTON] = true;
}

bool InputManager::isKeyDown(const EKEY_CODE keyCode)
{
	return keys[keyCode];
}

void InputManager::resetCode(const EKEY_CODE keyCode)
{
	keys[keyCode] = false;
}