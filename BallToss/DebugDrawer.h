#pragma once
#include "LinearMath/btIDebugDraw.h"
#include <irrlicht.h>

class DebugDrawer final : public btIDebugDraw
{
public:
	DebugDrawer(irr::video::IVideoDriver* videoDriver);
	
	// Debug mode functions
	void setDebugMode(const int debugMode) override { debuggingMode = debugMode; }
	int getDebugMode() const override { return debuggingMode; }

	// Drawing functions
	void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override;
	void drawContactPoint(const btVector3& pointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) override;

	// Unused
	void reportErrorWarning(const char* warningString) override {};
	void draw3dText(const btVector3& location, const char* textString) override {};

	void toggleDebugFlag(int flag);

protected:
	int debuggingMode;
	irr::video::IVideoDriver* videoDriver;
};