#include "IrrlichtGraphicsObject.h"
using namespace irr;
using namespace core;
using namespace scene;

extern IrrlichtDevice* device;

IrrlichtGraphicsObject::IrrlichtGraphicsObject()
{
	objectAnimatedMesh = nullptr;
	objectSceneNode = nullptr;
}

void IrrlichtGraphicsObject::setGraphicsPosition(const vector3df& v) const
{
	if (objectSceneNode) objectSceneNode->setPosition(v);
}

void IrrlichtGraphicsObject::setGraphicsRotation(const vector3df& v) const
{
	if (objectSceneNode) objectSceneNode->setRotation(v);
}

bool IrrlichtGraphicsObject::loadMeshFromFile(const c8* fileName)
{
	objectAnimatedMesh = device->getSceneManager()->getMesh(fileName);

	if (!objectAnimatedMesh)
		return false;

	objectSceneNode = device->getSceneManager()->addAnimatedMeshSceneNode(objectAnimatedMesh);

	if (!objectSceneNode)
		return false;

	IShadowVolumeSceneNode* shadowNode = ((IAnimatedMeshSceneNode*)objectSceneNode)->addShadowVolumeSceneNode();
	
	return true;
}

aabbox3d<f32> IrrlichtGraphicsObject::getBoundingBox() const
{
	aabbox3d<f32> extent;
	if (objectSceneNode) extent = objectSceneNode->getBoundingBox();

	if (extent.isEmpty())
	{
		extent.addInternalPoint(vector3df(-0.5f, -0.5f, -0.5f));
		extent.addInternalPoint(vector3df(0.5f, 0.5f, 0.5f));
	}

	return extent;
}