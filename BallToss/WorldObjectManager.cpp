#include "WorldObjectManager.h"

WorldObjectManager::WorldObjectManager()
{
	worldObjects.clear();
}

void WorldObjectManager::addWorldObject(WorldObject* obj)
{
	worldObjects.push_back(obj);
}

void WorldObjectManager::removeWorldObject(WorldObject* obj)
{
	for (auto iter = worldObjects.begin(); iter != worldObjects.end(); ++iter)
	{
		if (obj == *iter)
		{
			obj->cleanUp();
			delete obj;
			worldObjects.erase(iter);
			break;
		}
	}
}

void WorldObjectManager::updateAllWorldObjects()
{
	for (auto it = worldObjects.begin(); it != worldObjects.end(); ++it)
	{
		WorldObject* obj = *it;
		obj->updateTransform();
	}
}