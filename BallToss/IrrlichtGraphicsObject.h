#pragma once
#include <irrlicht.h>

class IrrlichtGraphicsObject
{
public:

	IrrlichtGraphicsObject();
	
	void setGraphicsPosition(const irr::core::vector3df& v) const;
	void setGraphicsRotation(const irr::core::vector3df& v) const;
	
	// Use irrlicht to load mesh from file
	bool loadMeshFromFile(const irr::c8* fileName);

	irr::core::aabbox3d<irr::f32> getBoundingBox() const;

	irr::scene::ISceneNode* getObjectSceneNode() const { return objectSceneNode; }

protected:
	irr::scene::IAnimatedMesh* objectAnimatedMesh;
	irr::scene::ISceneNode* objectSceneNode;
};