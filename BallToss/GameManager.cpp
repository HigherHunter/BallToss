#include "GameManager.h"
#include "DefinedWorldObjects.h"
#include "SoundManager.h"
#include <cstdlib>

using namespace irr;
using namespace core;
using namespace video;
using namespace scene;

GameManager::GameManager(IrrlichtDevice* device, WorldObjectManager* worldObjectManager, GuiManager* guiManager)
{
	this->device = device;
	this->worldObjectManager = worldObjectManager;
	this->guiManager = guiManager;

	sphere = nullptr;
	characterModel = nullptr;

	starToDelete = nullptr;
	shouldDeleteStar = false;

	changedToRun = false;
	hasHit = false;
	menuActive = true;
	sliderState = false;
	isWorldCreated = false;
	canFire = true;

	score = 0;
	scoreIncrement = 0;

	ps = nullptr;
	shouldShowParticles = false;
	particlesStartTime = 0;
	ballStartTime = 0;

	createParticleSystem();

	countdownTimer = new Timer(165.f);

	SoundManager::getInstance()->playMenuMusic();
}

void GameManager::createScene()
{
	// Create terrain
	WorldObject* terrain = new Terrain("../media/models/terrain/terrain.obj");
	terrain->getObjectSceneNode()->setMaterialFlag(EMF_LIGHTING, false);
	terrain->getRigidBody()->setRestitution(0.5f);
	terrain->getRigidBody()->setFriction(0.5f);

	// Create water mesh
	IAnimatedMesh* waterMesh = device->getSceneManager()->addHillPlaneMesh("waterMesh",
		dimension2d<f32>(20, 20),
		dimension2d<u32>(200, 200), nullptr, 0,
		dimension2d<f32>(0, 0),
		dimension2d<f32>(100, 100));

	// Add water
	ISceneNode* waterNode = device->getSceneManager()->addWaterSurfaceSceneNode(waterMesh->getMesh(0), 1.0f, 300.0f, 5.0f);
	waterNode->setPosition(vector3df(0, -10, 0));
	waterNode->setMaterialTexture(1, device->getVideoDriver()->getTexture("../media/textures/water.jpg"));
	waterNode->setMaterialTexture(0, device->getVideoDriver()->getTexture("../media/textures/sand.jpg"));
	waterNode->setMaterialFlag(EMF_LIGHTING, true);
	waterNode->setMaterialType(EMT_REFLECTION_2_LAYER);
	waterMesh->drop();

	// Create static object arc
	WorldObject* arc = new Concave(
		"../media/models/arc/archway_3.obj",
		"../media/models/arc/archway_3_Base_Color.jpg",
		"");
	arc->setScale(vector3df(0.1f, 0.1f, 0.1f));
	arc->setPosition(vector3df(20, -10, 120));
	arc->setRotation(vector3df(0, 90, 0));
	// Normalize normals since we have scaled the model, so that light can work properly
	arc->getObjectSceneNode()->setMaterialFlag(EMF_NORMALIZE_NORMALS, true);

	// Create static object pillar
	WorldObject* pillar = new Concave(
		"../media/models/pillar/egypt_pillar.obj",
		"../media/models/pillar/pillarTexture.png",
		"../media/models/pillar/pillarNormal.png");
	pillar->setScale(vector3df(10.f, 10.f, 10.f));
	pillar->setPosition(vector3df(0, 18, 0));
	// Normalize normals since we have scaled the model, so that light can work properly
	pillar->getObjectSceneNode()->setMaterialFlag(EMF_NORMALIZE_NORMALS, true);

	// First create the player character for edit
	createPlayerCharacter();

	// One big triangle selector
	IMetaTriangleSelector* selector = device->getSceneManager()->createMetaTriangleSelector();
	
	if (terrain)
	{
		IMeshSceneNode* terrainMeshNode = (IMeshSceneNode*)terrain->getObjectSceneNode();

		ITriangleSelector* terrainSelector = device->getSceneManager()->createOctreeTriangleSelector(
			terrainMeshNode->getMesh(), terrainMeshNode, 128);

		selector->addTriangleSelector(terrainSelector);
		terrainSelector->drop();
	}

	if (arc)
	{
		IAnimatedMeshSceneNode* arcMeshNode = (IAnimatedMeshSceneNode*)arc->getObjectSceneNode();

		ITriangleSelector* arcSelector = device->getSceneManager()->createTriangleSelector(arcMeshNode->getMesh()->getMesh(0), arcMeshNode);

		selector->addTriangleSelector(arcSelector);
		arcSelector->drop();
	}

	if (pillar)
	{
		IMeshSceneNode* pillarMeshNode = (IMeshSceneNode*)pillar->getObjectSceneNode();
		
		ITriangleSelector* pillarSelector = device->getSceneManager()->createTriangleSelector(pillarMeshNode->getMesh(), pillarMeshNode);

		selector->addTriangleSelector(pillarSelector);
		pillarSelector->drop();
	}
	
	if (selector)
	{
		ISceneNodeAnimator* anim = device->getSceneManager()->createCollisionResponseAnimator(
			selector, characterModel, vector3df(1, 3, 1), vector3df(0, -10, 0));
		selector->drop(); // As soon as we're done with the selector, drop it.
		characterModel->addAnimator(anim);
		anim->drop();  // And likewise, drop the animator when we're done referring to it.
	}
	
	createBall();

	createStars();

	isWorldCreated = true;

	guiManager->hideLoadingText();
}

void GameManager::moveBall(const vector3df& position) const
{
	sphere->getRigidBody()->setLinearVelocity(btVector3(0, 0, 0));
	sphere->getRigidBody()->setAngularVelocity(btVector3(0, 0, 0));
	sphere->setPosition(position);
}

void GameManager::fireBall(const vector3df& cameraPos, const vector3df& cameraLookAt, const float forceAmount) const
{
	moveBall(cameraPos);
	sphere->getRigidBody()->forceActivationState(ACTIVE_TAG);
	sphere->getRigidBody()->setDeactivationTime(0.f);
	const vector3df pos = sphere->getPhysicsPosition();

	const vector3df dir = vector3df(cameraLookAt - pos).normalize();
	const btVector3 direction = btVector3(dir.X, dir.Y, dir.Z);

	sphere->getRigidBody()->applyImpulse(forceAmount * direction, btVector3(0, 0, 0));
}

bool GameManager::canFireBall() const
{
	if (canFire && device->getTimer()->getRealTime() * 0.001f - ballStartTime > 3)
		return true;
	return false;
}

void GameManager::movePlayerCharacterInDirection(vector3df dir)
{
	if (!changedToRun)
	{
		characterModel->setMD2Animation(EMAT_RUN);
		characterModel->setAnimationSpeed(20.f);
		changedToRun = true;
	}
	vector3df pos = characterModel->getPosition();

	// Get direction
	matrix4 m;
	m.setRotationDegrees(characterModel->getRotation());
	m.rotateVect(dir);
	// Now dir matches node orientation
	pos += dir * 0.5f;
	characterModel->setPosition(pos);
}

void GameManager::movePlayerForward()
{
	movePlayerCharacterInDirection(vector3df(1, 0, 0));
}

void GameManager::movePlayerBackward()
{
	movePlayerCharacterInDirection(vector3df(-1, 0, 0));
}

void GameManager::stopPlayer()
{
	changedToRun = false;
	characterModel->setMD2Animation(EMAT_STAND);
}

void GameManager::resetPlayerPos() const
{
	characterModel->setPosition(vector3df(-20, 30, 120)); // Put its feet on the floor.
}

void GameManager::rotateStars()
{
	for (auto iter = starObjects.begin(); iter != starObjects.end(); ++iter)
	{
		vector3df rot = (*iter)->getObjectSceneNode()->getRotation();
		rot.Y += 1.f;
		if (rot.Y > 360.f)
			rot.Y = 0.f;
		(*iter)->setRotation(rot);
	}
}

void GameManager::restartGame()
{
	removeAllStars();
	createStars();
	resetPlayerPos();

	score = 0;
	scoreIncrement = 0;
	shouldDeleteStar = false;
	hasHit = false;

	guiManager->changeScore(score);

	device->getCursorControl()->setVisible(false);

	resetTimer();

	SoundManager::getInstance()->stopMenuMusic();
	SoundManager::getInstance()->playBackgroundMusic();

	guiManager->hideLoadingText();
}

void GameManager::showParticles()
{
	if (shouldShowParticles)
	{
		if (device->getTimer()->getRealTime() * 0.001f - particlesStartTime > 4)
		{
			shouldShowParticles = false;
			ps->removeAllAffectors();
		}
		else
			ps->setVisible(true);

		if (shouldDeleteStar)
			if (device->getTimer()->getRealTime() * 0.001f - particlesStartTime > 2)
			{
				if (starToDelete)
					removeStar(starToDelete);
				shouldDeleteStar = false;
			}
	}
	else
	{
		ps->setVisible(false);
	}
}

void GameManager::checkCollidedObject(btRigidBody* body0, btRigidBody* body1)
{

	if (body0 == sphere->getRigidBody())
	{
		WorldObject* obj = findStarObject(body1);
		Star* star = (Star*)obj;

		if (star)
		{
			if (star->isHittable())
			{
				hasHit = true;

				SoundManager::getInstance()->playScoreSound();

				score += star->getScoreValue();
				score += scoreIncrement;
				guiManager->changeScore(score);

				if (hasHit)
					scoreIncrement += 25;

				// Display particles
				ps->setPosition(star->getPhysicsPosition());
				shouldShowParticles = true;
				particlesStartTime = device->getTimer()->getRealTime() * 0.001f;
				ballStartTime = device->getTimer()->getRealTime() * 0.001f;
				canFire = true;
				
				IParticleAffector* gravity = ps->createGravityAffector(vector3df(0, 0.002f, 0), 1);
				ps->addAffector(gravity);
				gravity->drop();

				IParticleAffector* fade = ps->createFadeOutParticleAffector(SColor(0, 0, 0, 0), 5000);
				ps->addAffector(fade);
				fade->drop();

				star->setHittable(false);

				starToDelete = star;

				shouldDeleteStar = true;
			}
		}
		else if (body1)
		{
			if (!canFire)
				ballStartTime = device->getTimer()->getRealTime() * 0.001f;
			canFire = true;
		}
	}
}

WorldObject* GameManager::findStarObject(btRigidBody* body)
{
	for (auto iter = starObjects.begin(); iter != starObjects.end(); ++iter)
	{
		if ((*iter)->getRigidBody() == body)
		{
			// Found the body, so return the corresponding game object
			return *iter;
		}
	}

	return nullptr;
}

void GameManager::createPlayerCharacter()
{
	// Add an MD2 node, which uses vertex-based animation.
	characterModel = device->getSceneManager()->addAnimatedMeshSceneNode(device->getSceneManager()->getMesh("../media/models/character/faerie.md2"));
	characterModel->setPosition(vector3df(-20, 10, 120)); // Put its feet on the floor.
	characterModel->setScale(vector3df(0.1f)); // Make it appear realistically scaled
	characterModel->setMD2Animation(EMAT_STAND);
	characterModel->setAnimationSpeed(20.f);
	characterModel->setMaterialTexture(0, device->getVideoDriver()->getTexture("../media/models/character/faerie2.bmp"));
	characterModel->setMaterialFlag(EMF_LIGHTING, false);
}

void GameManager::createBall()
{
	sphere = new Sphere("../media/models/sphere/globe-sphere.obj", 1.f);
	sphere->setPosition(vector3df(-200, 10, 120));
	sphere->setScale(vector3df(2, 2, 2));
	sphere->setMass(20.f);
	sphere->getRigidBody()->setRestitution(0.5f);
	sphere->getRigidBody()->setFriction(0.7f);
	sphere->getRigidBody()->setRollingFriction(0.7f);
	sphere->getRigidBody()->setDamping(0.0f, 0.2f);
	sphere->getObjectSceneNode()->setMaterialFlag(EMF_LIGHTING, false);
	sphere->getObjectSceneNode()->setMaterialTexture(0, device->getVideoDriver()->getTexture("../media/textures/lava.jpg"));
	worldObjectManager->addWorldObject(sphere);
}

void GameManager::createStars()
{
	// Yellow
	//1
	WorldObject* star = new Star("../media/models/star/20facestar.obj", "../media/textures/yellow.jpg", 100);
	star->setPosition(vector3df(20, 0, 120));
	star->setRotation(vector3df(90, getRandomRotationNumber(), 0));
	starObjects.push_back(star);

	//2
	star = new Star("../media/models/star/20facestar.obj", "../media/textures/yellow.jpg", 100);
	star->setPosition(vector3df(60, 0, 120));
	star->setRotation(vector3df(90, getRandomRotationNumber(), 0));
	starObjects.push_back(star);

	//3
	star = new Star("../media/models/star/20facestar.obj", "../media/textures/yellow.jpg", 100);
	star->setPosition(vector3df(90, 10, 100));
	star->setRotation(vector3df(90, getRandomRotationNumber(), 0));
	starObjects.push_back(star);

	//4
	star = new Star("../media/models/star/20facestar.obj", "../media/textures/yellow.jpg", 100);
	star->setPosition(vector3df(100, 0, 80));
	star->setRotation(vector3df(90, getRandomRotationNumber(), 0));
	starObjects.push_back(star);

	//5
	star = new Star("../media/models/star/20facestar.obj", "../media/textures/yellow.jpg", 100);
	star->setPosition(vector3df(85, 5, 50));
	star->setRotation(vector3df(90, getRandomRotationNumber(), 0));
	starObjects.push_back(star);

	//6
	star = new Star("../media/models/star/20facestar.obj", "../media/textures/yellow.jpg", 100);
	star->setPosition(vector3df(60, 10, 30));
	star->setRotation(vector3df(90, getRandomRotationNumber(), 0));
	starObjects.push_back(star);

	//7
	star = new Star("../media/models/star/20facestar.obj", "../media/textures/yellow.jpg", 100);
	star->setPosition(vector3df(40, 15, 20));
	star->setRotation(vector3df(90, getRandomRotationNumber(), 0));
	starObjects.push_back(star);

	//8
	star = new Star("../media/models/star/20facestar.obj", "../media/textures/yellow.jpg", 100);
	star->setPosition(vector3df(20, 20, 20));
	star->setRotation(vector3df(90, getRandomRotationNumber(), 0));
	starObjects.push_back(star);

	//9
	star = new Star("../media/models/star/20facestar.obj", "../media/textures/yellow.jpg", 100);
	star->setPosition(vector3df(15, 22, 22));
	star->setRotation(vector3df(90, getRandomRotationNumber(), 0));
	starObjects.push_back(star);

	//10
	star = new Star("../media/models/star/20facestar.obj", "../media/textures/yellow.jpg", 100);
	star->setPosition(vector3df(20, 25, 0));
	star->setRotation(vector3df(90, getRandomRotationNumber(), 0));
	starObjects.push_back(star);

	//11
	star = new Star("../media/models/star/20facestar.obj", "../media/textures/yellow.jpg", 100);
	star->setPosition(vector3df(10, 20, -10));
	star->setRotation(vector3df(90, getRandomRotationNumber(), 0));
	starObjects.push_back(star);

	//12
	star = new Star("../media/models/star/20facestar.obj", "../media/textures/yellow.jpg", 100);
	star->setPosition(vector3df(0, 30, -5));
	star->setRotation(vector3df(90, getRandomRotationNumber(), 0));
	starObjects.push_back(star);

	//13
	star = new Star("../media/models/star/20facestar.obj", "../media/textures/yellow.jpg", 100);
	star->setPosition(vector3df(-20, 20, 0));
	star->setRotation(vector3df(90, getRandomRotationNumber(), 0));
	starObjects.push_back(star);

	//14
	star = new Star("../media/models/star/20facestar.obj", "../media/textures/yellow.jpg", 100);
	star->setPosition(vector3df(-25, 15, 10));
	star->setRotation(vector3df(90, getRandomRotationNumber(), 0));
	starObjects.push_back(star);

	//15
	star = new Star("../media/models/star/20facestar.obj", "../media/textures/yellow.jpg", 100);
	star->setPosition(vector3df(-15, 24, 15));
	star->setRotation(vector3df(90, getRandomRotationNumber(), 0));
	starObjects.push_back(star);

	//16
	star = new Star("../media/models/star/20facestar.obj", "../media/textures/yellow.jpg", 100);
	star->setPosition(vector3df(-25, 25, -18));
	star->setRotation(vector3df(90, getRandomRotationNumber(), 0));
	starObjects.push_back(star);

	//17
	star = new Star("../media/models/star/20facestar.obj", "../media/textures/yellow.jpg", 100);
	star->setPosition(vector3df(-10, 28, -26));
	star->setRotation(vector3df(90, getRandomRotationNumber(), 0));
	starObjects.push_back(star);

	//18
	star = new Star("../media/models/star/20facestar.obj", "../media/textures/yellow.jpg", 100);
	star->setPosition(vector3df(0, 20, -22));
	star->setRotation(vector3df(90, getRandomRotationNumber(), 0));
	starObjects.push_back(star);

	//19
	star = new Star("../media/models/star/20facestar.obj", "../media/textures/yellow.jpg", 100);
	star->setPosition(vector3df(40, 30, -15));
	star->setRotation(vector3df(90, getRandomRotationNumber(), 0));
	starObjects.push_back(star);

	//20
	star = new Star("../media/models/star/20facestar.obj", "../media/textures/yellow.jpg", 100);
	star->setPosition(vector3df(30, 25, -25));
	star->setRotation(vector3df(90, getRandomRotationNumber(), 0));
	starObjects.push_back(star);

	////////////////////////////////////////////////////////////

	//21
	star = new Star("../media/models/star/20facestar.obj", "../media/textures/yellow.jpg", 100);
	star->setPosition(vector3df(-70, 10, 40));
	star->setRotation(vector3df(90, getRandomRotationNumber(), 0));
	starObjects.push_back(star);

	//22
	star = new Star("../media/models/star/20facestar.obj", "../media/textures/yellow.jpg", 100);
	star->setPosition(vector3df(-80, 15, -20));
	star->setRotation(vector3df(90, getRandomRotationNumber(), 0));
	starObjects.push_back(star);

	//23
	star = new Star("../media/models/star/20facestar.obj", "../media/textures/yellow.jpg", 100);
	star->setPosition(vector3df(-40, 10, -80));
	star->setRotation(vector3df(90, getRandomRotationNumber(), 0));
	starObjects.push_back(star);

	//24
	star = new Star("../media/models/star/20facestar.obj", "../media/textures/yellow.jpg", 100);
	star->setPosition(vector3df(-15, 20, -85));
	star->setRotation(vector3df(90, getRandomRotationNumber(), 0));
	starObjects.push_back(star);

	//25
	star = new Star("../media/models/star/20facestar.obj", "../media/textures/yellow.jpg", 100);
	star->setPosition(vector3df(10, 30, -80));
	star->setRotation(vector3df(90, getRandomRotationNumber(), 0));
	starObjects.push_back(star);

	// Orange
	//1
	star = new Star("../media/models/star/20facestar.obj", "../media/textures/darkOrange.jpg", 200);
	star->setPosition(vector3df(0, 25, 20));
	star->setRotation(vector3df(90, getRandomRotationNumber(), 0));
	starObjects.push_back(star);

	//2
	star = new Star("../media/models/star/20facestar.obj", "../media/textures/darkOrange.jpg", 200);
	star->setPosition(vector3df(0, 40, 15));
	star->setRotation(vector3df(90, getRandomRotationNumber(), 0));
	starObjects.push_back(star);

	//3
	star = new Star("../media/models/star/20facestar.obj", "../media/textures/darkOrange.jpg", 200);
	star->setPosition(vector3df(0, 32, -45));
	star->setRotation(vector3df(90, getRandomRotationNumber(), 0));
	starObjects.push_back(star);

	// Red
	//1
	star = new Star("../media/models/star/20facestar.obj", "../media/textures/red.jpg", 300);
	star->setPosition(vector3df(20, 40, 30));
	star->setRotation(vector3df(90, getRandomRotationNumber(), 0));
	starObjects.push_back(star);

	//2
	star = new Star("../media/models/star/20facestar.obj", "../media/textures/red.jpg", 300);
	star->setPosition(vector3df(0, 50, 0));
	star->setRotation(vector3df(90, getRandomRotationNumber(), 0));
	starObjects.push_back(star);

	//3
	star = new Star("../media/models/star/20facestar.obj", "../media/textures/red.jpg", 300);
	star->setPosition(vector3df(-85, 10, -70));
	star->setRotation(vector3df(90, getRandomRotationNumber(), 0));
	starObjects.push_back(star);
}

void GameManager::removeStar(WorldObject* obj)
{
	for (auto iter = starObjects.begin(); iter != starObjects.end(); ++iter)
	{
		if ((*iter) == obj)
		{
			(obj)->cleanUp();
			delete (obj);
			starObjects.erase(iter);
			break;
		}
	}
}

void GameManager::removeAllStars()
{
	for (auto iter = starObjects.begin(); iter != starObjects.end(); ++iter)
	{
		WorldObject* obj = (*iter);
		if (obj)
		{
			obj->cleanUp();
			delete obj;
		}
	}
	starObjects.clear();
}

void GameManager::createParticleSystem()
{
	// Create a particle system
	ps = device->getSceneManager()->addParticleSystemSceneNode(false);

	IParticleEmitter* em = ps->createBoxEmitter(
		core::aabbox3d<f32>(-3, 0, -3, 3, 3, 3), // emitter size
		core::vector3df(0.01f, 0.03f, 0.01f),   // initial direction
		30, 60,                             // emit rate
		video::SColor(0, 200, 200, 0),       // darkest color
		video::SColor(0, 255, 255, 0),       // brightest color
		1000, 3000, 90,                         // min and max age, angle
		core::dimension2df(0.1f, 0.1f),         // min size
		core::dimension2df(1.f, 1.f));        // max size

	ps->setEmitter(em); // this grabs the emitter
	em->drop(); // so we can drop it here without deleting it

	ps->setPosition(vector3df(0, 2, 120));
	ps->setScale(vector3df(2, 2, 2));
	ps->setMaterialFlag(EMF_LIGHTING, false);
	ps->setMaterialFlag(EMF_ZWRITE_ENABLE, false);
	ps->setMaterialTexture(0, device->getVideoDriver()->getTexture("../media/textures/star.png"));
	ps->setMaterialType(EMT_TRANSPARENT_ADD_COLOR);

	ps->setVisible(false);
}

int GameManager::getRandomRotationNumber()
{
	return std::rand() % (360 + 1 - 0) + 0;
}