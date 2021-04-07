#include "CollisionObjectManager.h"

CollisionObjectManager::CollisionObjectManager()
= default;

CollisionObject* CollisionObjectManager::add(btCollisionShape* collisionShape, btTriangleMesh* triangleMesh, void* irrMesh)
{
	CollisionObject* newObject = new CollisionObject(collisionShape, triangleMesh, irrMesh);
	collisionObjects.push_back(newObject);
	return newObject;
}

CollisionObject* CollisionObjectManager::getCollisionObject(void* irrMesh)
{
	for (auto it = collisionObjects.begin(); it != collisionObjects.end(); ++it)
	{
		CollisionObject* currentObject = *it;
		if (currentObject->getIrrMeshPointer() == irrMesh)
			return currentObject;
	}
	return nullptr;
}