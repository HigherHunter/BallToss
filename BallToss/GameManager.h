#pragma once
#include <irrlicht.h>
#include "WorldObjectManager.h"
#include "GuiManager.h"
#include "Timer.h"

class GameManager
{
public:
	GameManager(irr::IrrlichtDevice* device, WorldObjectManager* worldObjectManager, GuiManager* guiManager);

	void createScene();

	void moveBall(const irr::core::vector3df& position) const;
	void fireBall(const irr::core::vector3df& cameraPos, const irr::core::vector3df& cameraLookAt, const float forceAmount) const;
	bool canFireBall() const;
	void setCanFire(const bool value) { canFire = value; }
	void movePlayerForward();
	void movePlayerBackward();
	void stopPlayer();
	void resetPlayerPos() const;
	irr::core::vector3df getPlayerCharacterPos() const { return characterModel->getPosition(); }
	void setPlayerCharacterRotation(const irr::core::vector3df& rot) const { characterModel->setRotation(rot); }
	
	void setHasHit(const bool value) { hasHit = value; }
	bool getHasHit() const { return hasHit; }
	
	int getScore() const { return score; }
	void resetIncrement() { scoreIncrement = 0; }

	void rotateStars();

	bool areAllStartsHit() const { return starObjects.empty(); }
	
	void restartGame();

	bool getMenuActiveState() const { return menuActive; }
	void setMenuActiveState(const bool state) { menuActive = state; }

	bool getSliderState() const { return sliderState; }
	void setSliderState(const bool state) { sliderState = state; }

	bool getIsWorldCreated() const { return isWorldCreated; }
	
	void showParticles();

	Timer* getTimer() const { return countdownTimer; }
	
	void checkCollidedObject(btRigidBody* body0, btRigidBody* body1);
	
private:
	irr::IrrlichtDevice* device;
	WorldObjectManager* worldObjectManager;
	GuiManager* guiManager;

	// Player controlled sphere
	WorldObject* sphere;

	// Player character
	irr::scene::IAnimatedMeshSceneNode* characterModel;

	bool changedToRun;
	
	WorldObject* starToDelete;
	bool shouldDeleteStar;

	std::vector<WorldObject*> starObjects;
	
	bool hasHit;
	
	bool menuActive;
	
	bool sliderState;

	bool isWorldCreated;

	bool canFire;
	
	int score;
	int scoreIncrement;

	irr::scene::IParticleSystemSceneNode* ps;
	bool shouldShowParticles;
	irr::u32 particlesStartTime;

	irr::u32 ballStartTime;
	
	Timer* countdownTimer;
	
	// Functions
	WorldObject* findStarObject(btRigidBody* body);

	void createPlayerCharacter();
	void createBall();
	void createStars();
	void removeStar(WorldObject* obj);
	void removeAllStars();
	void movePlayerCharacterInDirection(irr::core::vector3df dir);
	
	void resetTimer() const { countdownTimer->setTime(165.f); }
	
	void createParticleSystem();

	static int getRandomRotationNumber();
};