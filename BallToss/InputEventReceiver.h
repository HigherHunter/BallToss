#pragma once
#include <irrlicht.h>
#include "InputManager.h"
#include "GuiManager.h"

class InputEventReceiver : public irr::IEventReceiver
{
public:

	InputEventReceiver(InputManager* inputManager, GuiManager* guiManager);
	~InputEventReceiver();

	// Event method that must be implemented
	bool OnEvent(const irr::SEvent& event) override;

private:
	InputManager* inputManager;
	GuiManager* guiManager;
};