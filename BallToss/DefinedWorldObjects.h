#pragma once
#include "WorldObject.h"

class Box : public WorldObject
{
public:
	Box(const char* fileName, float mass);
};

class Sphere : public WorldObject
{
public:
	Sphere(const char* fileName, float mass);
};

class Concave : public WorldObject
{
public:
	Concave(const char* meshFile, const char* textureMap, const char* normalMap);
};

class Terrain : public WorldObject
{
public:
	Terrain(const char* fileName);
};

class Star : public WorldObject
{
public:
	Star(const char* fileName, const char* texture, int scoreAmount);

	int getScoreValue() const { return scoreAmount; }
	bool isHittable() const { return hittable; }
	void setHittable(const bool value) { hittable = value; }
	
private:
	int scoreAmount;
	bool hittable;
};