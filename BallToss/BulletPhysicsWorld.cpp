#include "BulletPhysicsWorld.h"
#include "CollisionObjectManager.h"
#include "GameManager.h"
#include <iterator>

extern GameManager* gameManager;

// Create the collision object manage for collision shape sharing
CollisionObjectManager* collisionObjectManager = nullptr;

BulletPhysicsWorld::BulletPhysicsWorld(WorldObjectManager* worldObjectManager)
{
	collisionConfiguration = new btDefaultCollisionConfiguration();
	collisionDispatcher = new btCollisionDispatcher(collisionConfiguration);
	broadphase = new btDbvtBroadphase();
	constraintSolver = new btSequentialImpulseConstraintSolver();
	dynamicsWorld = new btDiscreteDynamicsWorld(collisionDispatcher, broadphase, constraintSolver, collisionConfiguration);

	this->worldObjectManager = worldObjectManager;
	
	collisionObjectManager = new CollisionObjectManager();
}

BulletPhysicsWorld::~BulletPhysicsWorld()
{
	delete dynamicsWorld;
	delete constraintSolver;
	delete broadphase;
	delete collisionDispatcher;
	delete collisionObjectManager;
}

void BulletPhysicsWorld::updatePhysics(const float elapsedTime)
{
	dynamicsWorld->stepSimulation(elapsedTime, 1);

	checkForCollisionEvents();
	
	// Update object graphics according to transform
	worldObjectManager->updateAllWorldObjects();
}

void BulletPhysicsWorld::checkForCollisionEvents()
{
	// Keep a list of the collision pairs we
	// found during the current update
	collision_pairs pairsThisUpdate;

	// Iterate through all of the manifolds in the dispatcher
	for (int i = 0; i < collisionDispatcher->getNumManifolds(); ++i)
	{
		// Get the manifold
		btPersistentManifold* manifold = collisionDispatcher->getManifoldByIndexInternal(i);

		// Ignore manifolds that have no contact points.
		if (manifold->getNumContacts() > 0)
		{
			// Get the two rigid bodies involved in the collision
			const btRigidBody* body0 = static_cast<const btRigidBody*>(manifold->getBody0());
			const btRigidBody* body1 = static_cast<const btRigidBody*>(manifold->getBody1());

			// Always create the pair in a predictable order
			bool const swapped = body0 > body1;
			const btRigidBody* bodyA = swapped ? body1 : body0;
			const btRigidBody* bodyB = swapped ? body0 : body1;

			// Create the pair
			collision_pair thisPair = std::make_pair(bodyA, bodyB);

			// Insert the pair into the current list
			pairsThisUpdate.insert(thisPair);

			// If this pair doesn't exist in the list
			// from the previous update, it is a new
			// pair and we must send a collision event
			if (pairsLastUpdate.find(thisPair) == pairsLastUpdate.end())
				collisionEvent((btRigidBody*)body0, (btRigidBody*)body1);
		}
	}

	// Create another list for pairs that
	// were removed this update
	collision_pairs removedPairs;

	// This handy function gets the difference between
	// two sets. It takes the difference between
	// collision pairs from the last update, and this 
	// update and pushes them into the removed pairs list
	std::set_difference(pairsLastUpdate.begin(), pairsLastUpdate.end(),
		pairsThisUpdate.begin(), pairsThisUpdate.end(),
		std::inserter(removedPairs, removedPairs.begin()));

	// Iterate through all of the removed pairs
	// sending separation events for them
	for (collision_pairs::const_iterator iter = removedPairs.begin(); iter != removedPairs.end(); ++iter)
		separationEvent((btRigidBody*)iter->first, (btRigidBody*)iter->second);

	// In the next iteration we'll want to
	// compare against the pairs we found in this iteration
	pairsLastUpdate = pairsThisUpdate;
}

void BulletPhysicsWorld::collisionEvent(btRigidBody* body0, btRigidBody* body1)
{
	gameManager->checkCollidedObject(body0, body1);
}

void BulletPhysicsWorld::separationEvent(btRigidBody* body0, btRigidBody* body1)
{
}