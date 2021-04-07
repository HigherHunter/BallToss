#pragma once
#include <vector>
#include "btBulletDynamicsCommon.h"
#include "CollisionObject.h"

class CollisionObjectManager
{
public:
	CollisionObjectManager();

	CollisionObject* add(btCollisionShape* collisionShape, btTriangleMesh* triangleMesh, void* irrMesh);
	CollisionObject* getCollisionObject(void* irrMesh);
	
private:
	std::vector<CollisionObject*> collisionObjects;
};

extern CollisionObjectManager* collisionObjectManager;