#pragma once
#include "btBulletDynamicsCommon.h"
#include "WorldObjectManager.h"
#include <set>
#include <algorithm>

class BulletPhysicsWorld
{
public:
	BulletPhysicsWorld(WorldObjectManager* worldObjectManager);
	~BulletPhysicsWorld();

	void updatePhysics(float elapsedTime);	
	
	btDiscreteDynamicsWorld* getDynamicsWorld() const { return dynamicsWorld; }
	
private:
	// Core Bullet components
	btCollisionConfiguration* collisionConfiguration;
	btCollisionDispatcher* collisionDispatcher;
	btBroadphaseInterface* broadphase;
	btConstraintSolver* constraintSolver;
	btDiscreteDynamicsWorld* dynamicsWorld;

	WorldObjectManager* worldObjectManager;

	// Typedefs for collision event
	typedef std::pair<const btRigidBody*, const btRigidBody*> collision_pair;
	typedef  std::set<collision_pair> collision_pairs;

	collision_pairs pairsLastUpdate;

	// Collision event functions
	void checkForCollisionEvents();
	static void collisionEvent(btRigidBody* body0, btRigidBody* body1);
	static void separationEvent(btRigidBody* body0, btRigidBody* body1);
};