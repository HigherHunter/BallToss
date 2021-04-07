#pragma once
#include "btBulletDynamicsCommon.h"

class CollisionObject
{
public:
	CollisionObject(btCollisionShape* collisionShape, btTriangleMesh* triangleMesh, void* irrMesh);
	~CollisionObject();
	
	btCollisionShape* getCollisionShape() const { return collisionShape; }
	void* getIrrMeshPointer() const { return irrMesh; }

private:
	btCollisionShape* collisionShape;
	btTriangleMesh* triangleMesh; // This is not used by all objects
	void* irrMesh;
};