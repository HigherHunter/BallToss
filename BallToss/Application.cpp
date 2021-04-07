#include "Application.h"
#include "WorldObjectManager.h"
#include "DefinedWorldObjects.h"
#include "SoundManager.h"
#include "SaveManager.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace gui;

#define MAX_TIMESTEP 0.2f

// Current 60fps
#define MIN_STEP_SIZE 0.01666666f

// Irrlicht device
IrrlichtDevice* device = nullptr;

WorldObjectManager* worldObjectManager = nullptr;

GameManager* gameManager = nullptr;

InputManager* inputManager = nullptr;

SaveManager* saveManager = nullptr;

Application::Application()
{
	device = nullptr;
	videoDriver = nullptr;
	sceneManager = nullptr;
	guiEnvironment = nullptr;
	camera[0] = nullptr;
	camera[1] = nullptr;

	windowDimensions.Width = 1280;
	windowDimensions.Height = 720;

	direction = 0.f;
	yDirection = 0.f;
	
	physicsWorld = nullptr;

	inputManager = new InputManager();
	guiManager = new GuiManager();
	inputEventReceiver = new InputEventReceiver(inputManager, guiManager);

	guiManager->setWindowWidth(windowDimensions.Width);
	guiManager->setWindowHeight(windowDimensions.Height);

	debugDrawer = nullptr;
	isDebugActive = false;

	initializeGraphics();
	initializePhysics();
	initializeDebugger();

	gameManager = new GameManager(device, worldObjectManager, guiManager);

	saveManager = new SaveManager();
	saveManager->readHighScore();
}

Application::~Application()
= default;

bool Application::initializeGraphics()
{
	// Create irrlicht device.
	device = createDevice(EDT_DIRECT3D9,
		dimension2d<u32>(windowDimensions.Width, windowDimensions.Height),
		32, false, true, false, inputEventReceiver);

	if (device == nullptr) return false; // Could not create device.

	// Set window caption
	device->setWindowCaption(L"BallToss");
	
	// Disable mouse cursor
	device->getCursorControl()->setVisible(false);
	
	// Video driver
	videoDriver = device->getVideoDriver();
	videoDriver->setTextureCreationFlag(ETCF_ALWAYS_32_BIT, true);

	// Scene manager
	sceneManager = device->getSceneManager();

	// Gui
	guiEnvironment = device->getGUIEnvironment();
	// Custom font
	IGUISkin* skin = guiEnvironment->getSkin();

	IGUIFont* font = guiEnvironment->getFont("../media/fonts/fontCalibri24/myfont.xml");
	if (font)
		skin->setFont(font);

	// Independent camera made into third person camera
	camera[0] = sceneManager->addCameraSceneNode();
	camera[0]->setFarValue(42000.0f);

	// Camera - FPS style user controlled camera
	camera[1] = sceneManager->addCameraSceneNodeFPS(nullptr, 40.0f, 0.f);
	camera[1]->setFarValue(42000.0f);

	sceneManager->setActiveCamera(camera[0]);
	
	// Add a directional light
	ILightSceneNode* dirLight = sceneManager->addLightSceneNode(nullptr, vector3df(0, 1, 1), SColorf(1.0, 1.0, 1.0, 1.0), 6000.f);

	SLight& lightData = dirLight->getLightData();
	lightData.AmbientColor = SColorf(0.15f, 0.15f, 0.15f); //or is very dark on back faces
	
	// Add parent node to rotate directional light
	ISceneNode* parentNode = sceneManager->addEmptySceneNode();

	dirLight->setPosition(vector3df(0, 0, 1000.f)); //put at some distance
	dirLight->setParent(parentNode);

	parentNode->setRotation(vector3df(-120, 0, -30));

	// Only set the alpha component to change shadow intensity
	sceneManager->setShadowColor(SColor(64, 0, 0, 0));

	// Create skybox
	videoDriver->setTextureCreationFlag(ETCF_CREATE_MIP_MAPS, false);

	ISceneNode* skybox = sceneManager->addSkyBoxSceneNode(
		videoDriver->getTexture("../media/sky/up.jpg"),
		videoDriver->getTexture("../media/sky/down.jpg"),
		videoDriver->getTexture("../media/sky/left.jpg"),
		videoDriver->getTexture("../media/sky/right.jpg"),
		videoDriver->getTexture("../media/sky/forward.jpg"),
		videoDriver->getTexture("../media/sky/back.jpg"));

	videoDriver->setTextureCreationFlag(ETCF_CREATE_MIP_MAPS, true);

	guiManager->createGuiElements();
	
	return true;
}

bool Application::initializePhysics()
{
	worldObjectManager = new WorldObjectManager();
	physicsWorld = new BulletPhysicsWorld(worldObjectManager);

	BulletPhysicsObject::physicsWorld = physicsWorld;

	return true;
}

bool Application::initializeDebugger()
{
	// Create the debug drawer
	debugDrawer = new DebugDrawer(videoDriver);

	// Set initial debug level to 0
	debugDrawer->setDebugMode(0);
	// Add the debug drawer to the world

	// Add the debug drawer to the world
	physicsWorld->getDynamicsWorld()->setDebugDrawer(debugDrawer);

	// Toggle wireframe
	debugDrawer->toggleDebugFlag(btIDebugDraw::DBG_DrawWireframe);
	
	return true;
}

void Application::setActiveCamera(ICameraSceneNode* newActiveCamera) const
{
	if (device)
	{
		ICameraSceneNode* activeCamera = sceneManager->getActiveCamera();
		activeCamera->setInputReceiverEnabled(false);

		const vector3df dir = vector3df(activeCamera->getTarget() - activeCamera->getPosition()).normalize();
		vector3df cameraPos = newActiveCamera->getPosition();
		cameraPos += dir * 10.f;
		newActiveCamera->setTarget(cameraPos);
		
		sceneManager->setActiveCamera(newActiveCamera);
		newActiveCamera->setInputReceiverEnabled(true);
	}
}

void Application::moveCameraControl()
{
	const position2d<f32> cursorPos = device->getCursorControl()->getRelativePosition();
	ICameraSceneNode* currentCamera = camera[0];

	const float change_x = (cursorPos.X - 0.5) * 256.0f;
	const float change_y = (cursorPos.Y - 0.5) * 256.0f;
	direction += change_x;
	yDirection -= change_y;
	if (yDirection < -90)
		yDirection = -90;
	else
		if (yDirection > 90)
			yDirection = 90;
	device->getCursorControl()->setPosition(0.5f, 0.5f);

	const vector3df playerPos = gameManager->getPlayerCharacterPos();

	float xf = playerPos.X - cos(direction * PI / 180.0f) * 8.0f;
	float yf = playerPos.Y - sin(yDirection * PI / 180.0f) * 8.0f;
	if (yf < -2)
		yf = -2.f;
	float zf = playerPos.Z + sin(direction * PI / 180.0f) * 8.0f;

	currentCamera->setPosition(vector3df(xf, yf, zf));
	currentCamera->setTarget(vector3df(playerPos.X, playerPos.Y + 5.0f, playerPos.Z));
	gameManager->setPlayerCharacterRotation(vector3df(0, direction, 0));
}

void Application::runApp()
{	
	u32 then = device->getTimer()->getTime();
	u32 now = 0;
	f32 deltaTime = 0.f;
	f32 accumulator = 0.f;

	while (device->run())
	{
		if (device->isWindowActive() && !gameManager->getMenuActiveState())
		{
			device->getCursorControl()->setVisible(false);

			if (sceneManager->getActiveCamera() == camera[0])
				moveCameraControl();
			
			// Show pause menu on esc
			if (inputManager->isKeyDown(KEY_ESCAPE))
			{
				gameManager->setMenuActiveState(true);
				guiManager->showPauseMenu();
				guiManager->hideOtherGuiElements();
				inputManager->resetCode(KEY_ESCAPE);
			}
			
			// Show power bar and launch the ball
			if (inputManager->isKeyDown(KEY_LBUTTON))
			{
				if (gameManager->canFireBall())
				{
					if (!gameManager->getSliderState())
					{
						// FPS camera
						setActiveCamera(camera[1]);
						guiManager->setSliderVisible(true);
						gameManager->setSliderState(true);
					}
					else if (gameManager->getSliderState())
					{
						gameManager->moveBall(camera[1]->getPosition());
						
						if (!gameManager->getHasHit())
							gameManager->resetIncrement();

						const float powerValue = guiManager->getSliderValue() / 3;
						if (camera[1] && camera[1]->getPosition().Y < -11)
							SoundManager::getInstance()->playShootUnderWater();
						else
						{
							if (powerValue < 14)
								SoundManager::getInstance()->playShootBallSLow();
							else if (powerValue >= 14 && powerValue <= 28)
								SoundManager::getInstance()->playShootBallMed();
							else if (powerValue > 28)
								SoundManager::getInstance()->playShootBallFast();
						}
						gameManager->fireBall(camera[1]->getPosition(), camera[1]->getTarget(), powerValue);
						gameManager->setSliderState(false);
						guiManager->setSliderVisible(false);
						guiManager->resetSlider();
						gameManager->setHasHit(false);
						gameManager->setCanFire(false);
						
						setActiveCamera(camera[0]);
					}
				}
				inputManager->resetCode(KEY_LBUTTON);
			}

			now = device->getTimer()->getTime();
			deltaTime = (now - then) / 1000.f;
			then = now;

			accumulator += deltaTime;
			if (accumulator > MAX_TIMESTEP) accumulator = MAX_TIMESTEP;

			if (accumulator >= MIN_STEP_SIZE)
			{
				if (gameManager->getSliderState())
					guiManager->moveSlider();

				gameManager->showParticles();

				vector3df cameraPos;
				
				vector3df currentPlayerPos = gameManager->getPlayerCharacterPos();
				currentPlayerPos.Y += 3.f;
				if (camera[1])
				{
					camera[1]->setPosition(currentPlayerPos);
					cameraPos = camera[1]->getPosition();
				}
				
				if (cameraPos.Y < -11)
					guiManager->showUnderWater(true);
				else
					guiManager->showUnderWater(false);
				
				if (cameraPos.X < -200 || cameraPos.Z < -200 || cameraPos.X > 200 || cameraPos.Z > 200)
				{
					if (cameraPos.Y < -50)
						gameManager->resetPlayerPos();
				}

				gameManager->rotateStars();

				gameManager->getTimer()->updateTime();

				guiManager->updateTimerGui(gameManager->getTimer()->getTime());

				if (gameManager->getTimer()->getTime() <= 0 || gameManager->areAllStartsHit())
				{
					stringw messageText = "Level 1\n\n";
					if (gameManager->getScore() < 3000)
					{
						messageText += "Bronze Medal.\n\n";
						messageText += "(6000 points needed for Silver)\n\n";
					}
					else if (gameManager->getScore() > 5000 && gameManager->getScore() < 7000)
					{
						messageText += "Silver Medal.\n\n";
						messageText += "(8000 points needed for Gold)\n\n";
					}
					else if (gameManager->getScore() > 8000)
					{
						messageText += "Gold Medal.\n\n";
						messageText += "Congratulations!\n\n";
					}
					gameManager->moveBall(vector3df(-200, 0, 0));
					
					gameManager->setSliderState(false);
					guiManager->setSliderVisible(false);
					guiManager->resetSlider();
					gameManager->setHasHit(false);
					
					gameManager->setMenuActiveState(true);
					guiManager->showEndGameWindow(messageText);
					guiManager->hideOtherGuiElements();
					SoundManager::getInstance()->playEndGame();
					saveManager->setHighScore(guiManager->getPlayerName(), gameManager->getScore());
					saveManager->writeHighScore();
				}

				if (inputManager->isKeyDown(KEY_KEY_W))
				{
					gameManager->movePlayerForward();
				}
				else if (inputManager->isKeyDown(KEY_KEY_S))
				{
					gameManager->movePlayerBackward();
				}
				else
					gameManager->stopPlayer();
				
				physicsWorld->updatePhysics(accumulator);

				videoDriver->beginScene(true, true, 0);

				if (sceneManager) sceneManager->drawAll();
				if (guiEnvironment) guiEnvironment->drawAll();

				if (isDebugActive)
					physicsWorld->getDynamicsWorld()->debugDrawWorld();

				videoDriver->endScene();

				accumulator = 0.f;
			}
		}
		else if (gameManager->getMenuActiveState())
		{
			now = device->getTimer()->getTime();
			deltaTime = (now - then) / 1000.f;
			then = now;

			accumulator += deltaTime;
			if (accumulator > MAX_TIMESTEP) accumulator = MAX_TIMESTEP;

			if (accumulator >= MIN_STEP_SIZE)
			{
				device->getCursorControl()->setVisible(true);
				videoDriver->beginScene(true, true, 0);
				if (guiEnvironment) guiEnvironment->drawAll();
				videoDriver->endScene();

				accumulator = 0.f;
			}
		}
	}
}