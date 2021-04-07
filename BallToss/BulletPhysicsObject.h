#pragma once
#include <irrlicht.h>
#include <btBulletDynamicsCommon.h>
#include "CollisionObject.h"

class BulletPhysicsWorld;

class BulletPhysicsObject
{
public:
	BulletPhysicsObject();
	~BulletPhysicsObject();
	
	static BulletPhysicsWorld* physicsWorld;
	
	irr::core::vector3df getPhysicsPosition() const;
	void setPhysicsPosition(const irr::core::vector3df& v) const;
	
	irr::core::vector3df getPhysicsRotation() const;
	void setPhysicsRotation(const irr::core::vector3df& v) const;

	btRigidBody* getRigidBody() const { return rigidBody; }
	
protected:
	CollisionObject* collisionObject;
	float mass;

	// Functions
	void initObjectsPhysicsComponents();
	static btTriangleMesh* getTriangleMesh(irr::scene::IMesh* mesh);

private:
	btMotionState* motionState;
	btRigidBody* rigidBody;
};