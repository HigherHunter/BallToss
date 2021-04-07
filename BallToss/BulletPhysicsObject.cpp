#include "BulletPhysicsObject.h"
#include "BulletPhysicsWorld.h"

using namespace irr;
using namespace core;
using namespace video;
using namespace scene;

BulletPhysicsWorld* BulletPhysicsObject::physicsWorld = nullptr;

BulletPhysicsObject::BulletPhysicsObject()
{
	motionState = nullptr;
	rigidBody = nullptr;

	collisionObject = nullptr;
	mass = 0.f;
}

BulletPhysicsObject::~BulletPhysicsObject()
{
	delete motionState;
	
	if (physicsWorld)
		physicsWorld->getDynamicsWorld()->removeRigidBody(rigidBody);
}

vector3df BulletPhysicsObject::getPhysicsPosition() const
{
	if (!rigidBody) return vector3df(0.f, 0.f, 0.f);
	const btVector3 pos = rigidBody->getCenterOfMassPosition();
	return vector3df(pos.x(), pos.y(), pos.z());
}

void BulletPhysicsObject::setPhysicsPosition(const vector3df& v) const
{
	if (!rigidBody) return;
	btTransform transform = rigidBody->getWorldTransform();
	transform.setOrigin(btVector3(v.X, v.Y, v.Z));
	rigidBody->setWorldTransform(transform);
}

vector3df BulletPhysicsObject::getPhysicsRotation() const
{
	if (!rigidBody) return vector3df(0.f, 0.f, 0.f);
	const btQuaternion rot = rigidBody->getOrientation();
	const quaternion q(rot.x(), rot.y(), rot.z(), rot.w());
	vector3df euler;
	q.toEuler(euler);
	return euler *= RADTODEG;
}

void BulletPhysicsObject::setPhysicsRotation(const vector3df& v) const
{
	if (!rigidBody) return;
	btTransform transform = rigidBody->getWorldTransform();
	const btVector3 euler = btVector3(v.X * DEGTORAD, v.Y * DEGTORAD, v.Z * DEGTORAD);
	btQuaternion quaternion;

	btMatrix3x3 mat;
	mat.setIdentity();
	mat.setEulerZYX(euler.x(), euler.y(), euler.z());
	mat.getRotation(quaternion);

	transform.setRotation(quaternion);
	rigidBody->setWorldTransform(transform);
}

void BulletPhysicsObject::initObjectsPhysicsComponents()
{
	if (!collisionObject || !collisionObject->getCollisionShape()) return;

	if (!physicsWorld) return;

	btDiscreteDynamicsWorld* dynamicsWorld = physicsWorld->getDynamicsWorld();

	btTransform startTransform;
	startTransform.setIdentity();

	btVector3 localInertia(0, 0, 0);
	if (mass != 0)
		collisionObject->getCollisionShape()->calculateLocalInertia(mass, localInertia);

	motionState = new btDefaultMotionState(startTransform);

	rigidBody = new btRigidBody(mass, motionState, collisionObject->getCollisionShape(), localInertia);

	dynamicsWorld->addRigidBody(rigidBody);

	rigidBody->setUserPointer((void*)this);
}

btTriangleMesh* BulletPhysicsObject::getTriangleMesh(IMesh* mesh)
{
	btVector3 vertices[3];
	s32 index, numOfVertices;
	u16* meshBufferIndices;

	btTriangleMesh* triangleMesh = new btTriangleMesh();

	for (u32 i = 0; i < mesh->getMeshBufferCount(); i++)
	{
		IMeshBuffer* meshBuffer = mesh->getMeshBuffer(i);

		// Extract vertex data
		// Because the vertices are stored as struct with no common base class,
		// we need to handle each type separately
		if (meshBuffer->getVertexType() == EVT_STANDARD)
		{
			S3DVertex* meshBufferVertices = (S3DVertex*)meshBuffer->getVertices();
			meshBufferIndices = meshBuffer->getIndices();
			numOfVertices = meshBuffer->getVertexCount();

			for (u32 j = 0; j < meshBuffer->getIndexCount(); j += 3)
			{
				for (s32 k = 0; k < 3; k++) // Three verts per triangle
				{
					index = meshBufferIndices[j + k];
					if (index > numOfVertices) goto error;

					// Convert to btVector3
					vertices[k] = btVector3(
						meshBufferVertices[index].Pos.X, meshBufferVertices[index].Pos.Y, meshBufferVertices[index].Pos.Z);
				}
				triangleMesh->addTriangle(vertices[0], vertices[1], vertices[2]);
			}
		}
		// Same but for S3DVertex2TCoords
		else if (meshBuffer->getVertexType() == EVT_2TCOORDS)
		{
			S3DVertex2TCoords* meshBufferVertices = (S3DVertex2TCoords*)meshBuffer->getVertices();
			meshBufferIndices = meshBuffer->getIndices();
			numOfVertices = meshBuffer->getVertexCount();

			for (u32 j = 0; j < meshBuffer->getIndexCount(); j += 3)
			{
				for (s32 k = 0; k < 3; k++) // Three verts per triangle
				{
					index = meshBufferIndices[j + k];
					if (index > numOfVertices) goto error;

					// Convert to btVector3
					vertices[k] = btVector3(
						meshBufferVertices[index].Pos.X, meshBufferVertices[index].Pos.Y, meshBufferVertices[index].Pos.Z);
				}
				triangleMesh->addTriangle(vertices[0], vertices[1], vertices[2]);
			}
		}
	}

	return triangleMesh;
error:
	delete triangleMesh;
	return nullptr;
}