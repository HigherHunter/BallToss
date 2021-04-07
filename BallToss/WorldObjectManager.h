#pragma once
#include <vector>
#include "WorldObject.h"

class WorldObjectManager
{
public:
	WorldObjectManager();

	void addWorldObject(WorldObject* obj);
	void removeWorldObject(WorldObject* obj);

	void updateAllWorldObjects();

private:
	std::vector<WorldObject*> worldObjects;
	std::vector<WorldObject*>::iterator currentWorldObject;
};

extern WorldObjectManager* worldObjectManager;