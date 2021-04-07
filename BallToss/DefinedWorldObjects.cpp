#include <irrlicht.h>
#include "DefinedWorldObjects.h"
#include "CollisionObject.h"
#include "CollisionObjectManager.h"

using namespace irr;
using namespace core;
using namespace video;
using namespace scene;

extern IrrlichtDevice* device;

Box::Box(const char* fileName, const float mass)
{
	if (!loadMeshFromFile(fileName)) return;
	
	// If we have this kind of collision shape just get it
	// - Collision shapes are shared between identical meshes -
	collisionObject = collisionObjectManager->getCollisionObject((void*)objectAnimatedMesh);
	
	if (!collisionObject)
	{	
		// We don't have this kind of shape
		// We will make it from the loaded mesh
		const aabbox3d<f32> box = getBoundingBox();
		const vector3df extent = box.getExtent() * 0.5f;
		
		btCollisionShape* shape = new btBoxShape(btVector3(extent.X, extent.Y, extent.Z));
		
		// Save collision shape for future use
		collisionObject = collisionObjectManager->add(shape, nullptr, (void*)objectAnimatedMesh);
	}
	this->mass = mass;
	initObjectsPhysicsComponents();
}

Sphere::Sphere(const char* fileName, const float mass)
{
	if (!loadMeshFromFile(fileName)) return;

	// If we have this kind of collision shape just get it
	// - Collision shapes are shared between identical meshes -
	collisionObject = collisionObjectManager->getCollisionObject((void*)objectAnimatedMesh);

	if (!collisionObject)
	{
		// We don't have this kind of shape
		// We will make it from the loaded mesh
		const aabbox3d<f32> box = getBoundingBox();
		const vector3df extent = box.getExtent() * 0.5f;

		btCollisionShape* shape = new btSphereShape(extent.X);
		// Save collision shape for future use
		collisionObject = collisionObjectManager->add(shape, nullptr, (void*)objectAnimatedMesh);
	}
	this->mass = mass;
	initObjectsPhysicsComponents();
}

Concave::Concave(const char* meshFile, const char* textureMap, const char* normalMap)
{
	// If we have normal map loaded make special parallax object
	if (*normalMap != 0)
	{
		objectAnimatedMesh = device->getSceneManager()->getMesh(meshFile);
		if (objectAnimatedMesh)
		{
			// Use mesh manipulator
			IMeshManipulator* manipulator = device->getSceneManager()->getMeshManipulator();

			// Create mesh copy with tangent information from original mesh
			IMesh* tangentMesh = manipulator->createMeshWithTangents(objectAnimatedMesh->getMesh(0));
			
			objectSceneNode = device->getSceneManager()->addMeshSceneNode(tangentMesh);
			objectSceneNode->setMaterialTexture(0, device->getVideoDriver()->getTexture(textureMap));
			objectSceneNode->setMaterialTexture(1, device->getVideoDriver()->getTexture(normalMap));
			objectSceneNode->setMaterialType(EMT_PARALLAX_MAP_TRANSPARENT_VERTEX_ALPHA);
		}
		else { return; }
	}
	else
	{
		if (!loadMeshFromFile(meshFile)) return;

		objectSceneNode->setMaterialTexture(0, device->getVideoDriver()->getTexture(textureMap));
	}
	
	// If we have this kind of collision shape just get it
	// - Collision shapes are shared between identical meshes -
	collisionObject = collisionObjectManager->getCollisionObject((void*)objectAnimatedMesh);

	if (!collisionObject)
	{
		// We don't have this kind of shape
		// We will make it from the loaded mesh
		btTriangleMesh* mesh = getTriangleMesh(objectAnimatedMesh->getMesh(0));

		btCollisionShape* shape = new btBvhTriangleMeshShape(mesh, true);

		// Save collision shape for future use
		collisionObject = collisionObjectManager->add(shape, nullptr, (void*)objectAnimatedMesh);
	}
	mass = 0.f;
	initObjectsPhysicsComponents();
}

Terrain::Terrain(const char* fileName)
{
	objectAnimatedMesh = device->getSceneManager()->getMesh(fileName);
	objectSceneNode = device->getSceneManager()->addOctreeSceneNode(objectAnimatedMesh->getMesh(0));
	
	mass = 0.f; // Static object

	// If we have this kind of collision shape just get it
	// - Collision shapes are shared between identical meshes -
	collisionObject = collisionObjectManager->getCollisionObject((void*)objectAnimatedMesh);

	if (!collisionObject)
	{
		// We don't have this kind of shape
		// We will make it from the loaded mesh
		btTriangleMesh* triangleMesh = getTriangleMesh(objectAnimatedMesh->getMesh(0));

		btCollisionShape* collisionShape = new btBvhTriangleMeshShape((btStridingMeshInterface*)triangleMesh, true);
		
		collisionObject = collisionObjectManager->add(collisionShape, nullptr, (void*)objectAnimatedMesh);
	}
	initObjectsPhysicsComponents();
}

Star::Star(const char* fileName, const char* texture, const int scoreAmount)
{
	this->scoreAmount = scoreAmount;
	hittable = true;
	
	if (!loadMeshFromFile(fileName)) return;

	// Disable lighting effect
	objectSceneNode->setMaterialFlag(EMF_LIGHTING, false);

	objectSceneNode->setMaterialTexture(0, device->getVideoDriver()->getTexture(texture));
	
	// If we have this kind of collision shape just get it
	// - Collision shapes are shared between identical meshes -
	collisionObject = collisionObjectManager->getCollisionObject((void*)objectAnimatedMesh);

	if (!collisionObject)
	{
		// We don't have this kind of shape
		// We will make it from the loaded mesh
		const aabbox3d<f32> box = getBoundingBox();
		const vector3df extent = box.getExtent() * 0.5f;

		btCollisionShape* shape = new btBoxShape(btVector3(extent.X, extent.Y, extent.Z));

		// Save collision shape for future use
		collisionObject = collisionObjectManager->add(shape, nullptr, (void*)objectAnimatedMesh);
	}
	mass = 0.f;
	initObjectsPhysicsComponents();
}