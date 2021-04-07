#pragma once
#include <irrlicht.h>
#include "InputEventReceiver.h"
#include "InputManager.h"
#include "GuiManager.h"
#include "BulletPhysicsWorld.h"
#include "GameManager.h"
#include "DebugDrawer.h"

class Application
{
public:

	Application();
	~Application();

	void runApp();

private:
	// Core Irrlicht graphics components
	irr::video::IVideoDriver* videoDriver;
	irr::scene::ISceneManager* sceneManager;
	irr::gui::IGUIEnvironment* guiEnvironment;
	irr::scene::ICameraSceneNode* camera[2];

	irr::core::dimension2d<irr::u32> windowDimensions;

	// Directions for third-person camera
	float direction, yDirection;
	
	// Input receiver
	InputEventReceiver* inputEventReceiver;

	// Gui manager
	GuiManager* guiManager;

	// Core Bullet component
	BulletPhysicsWorld* physicsWorld;
	
	// Debug renderer
	DebugDrawer* debugDrawer;
	bool isDebugActive;
	
	// Functions
	bool initializeGraphics();
	bool initializePhysics();
	bool initializeDebugger();
	void setActiveCamera(irr::scene::ICameraSceneNode* newActiveCamera) const;
	void moveCameraControl();
};