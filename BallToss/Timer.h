#pragma once
#include <chrono>

class Timer
{
public:
	Timer(float startTime);

	void updateTime();
	float getTime() const { return totalTime; }
	void setTime(const float time) { totalTime = time; then = std::chrono::system_clock::now(); }

private:
	float totalTime;
	std::chrono::system_clock::time_point now, then;
};