#include "WorldObject.h"

void WorldObject::updateTransform() const
{
	setGraphicsPosition(getPhysicsPosition());
	setGraphicsRotation(getPhysicsRotation());
}

void WorldObject::cleanUp() const
{
	if (objectSceneNode) objectSceneNode->remove();
}

void WorldObject::setPosition(const irr::core::vector3df& v) const
{
	setPhysicsPosition(v);
	setGraphicsPosition(v);
}

void WorldObject::setRotation(const irr::core::vector3df& v) const
{
	setPhysicsRotation(v);
	setGraphicsRotation(v);
}

void WorldObject::setScale(const irr::core::vector3df& v) const
{
	objectSceneNode->setScale(v);
	collisionObject->getCollisionShape()->setLocalScaling(btVector3(v.X, v.Y, v.Z));
}