#include "DebugDrawer.h"
using namespace irr;
using namespace core;
using namespace video;

DebugDrawer::DebugDrawer(IVideoDriver* videoDriver)
{
	debuggingMode = 0;
	this->videoDriver = videoDriver;
}

void DebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
{
	const vector3df start = vector3df(from.getX(), from.getY(), from.getZ());
	const vector3df end = vector3df(to.getX(), to.getY(), to.getZ());
	const SColor drawColor = SColor(255, color.getX(), color.getY(), color.getZ());

	matrix4 id;
	id.makeIdentity();
	if (videoDriver)
	{
		videoDriver->setTransform(ETS_WORLD, id);
		videoDriver->draw3DLine(start, end, drawColor);
	}
}

void DebugDrawer::drawContactPoint(const btVector3& pointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
{
	// Draw a line between two contact points
	btVector3 const startPoint = pointOnB;
	btVector3 const endPoint = pointOnB + normalOnB * distance;
	drawLine(startPoint, endPoint, color);
}

void DebugDrawer::toggleDebugFlag(const int flag)
{
	if (debuggingMode & flag)
		// Flag is enabled, disable it
		debuggingMode = debuggingMode & (~flag);
	else
		// Flag is disabled, enable it
		debuggingMode |= flag;
}