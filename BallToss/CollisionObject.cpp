#include "CollisionObject.h"

CollisionObject::CollisionObject(btCollisionShape* collisionShape, btTriangleMesh* triangleMesh, void* irrMesh)
{
	this->collisionShape = collisionShape;
	this->triangleMesh = triangleMesh;
	this->irrMesh = irrMesh;
}

CollisionObject::~CollisionObject()
{
	delete triangleMesh;
	delete collisionShape;
}