#include "InputEventReceiver.h"
using namespace irr;

InputEventReceiver::InputEventReceiver(InputManager* inputManager, GuiManager* guiManager)
{
	this->inputManager = inputManager;
	this->guiManager = guiManager;
}

InputEventReceiver::~InputEventReceiver()
= default;

bool InputEventReceiver::OnEvent(const SEvent & event)
{
	if (event.EventType == EET_KEY_INPUT_EVENT || event.EventType == EET_MOUSE_INPUT_EVENT)
		inputManager->handleInput(event);
	else if (event.EventType == EET_GUI_EVENT)
		guiManager->handleInput(event);

	return false;
}