#pragma once
#include "IrrlichtGraphicsObject.h"
#include "BulletPhysicsObject.h"

class WorldObject : public IrrlichtGraphicsObject, public BulletPhysicsObject
{
public:
	
	void updateTransform() const;
	
	void cleanUp() const;

	void setPosition(const irr::core::vector3df& v) const;
	void setRotation(const irr::core::vector3df& v) const;
	void setScale(const irr::core::vector3df& v) const;

	void setMass(const float mass) { this->mass = mass; }
	float getMass() const { return mass; }
};