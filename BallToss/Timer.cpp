#include "Timer.h"

Timer::Timer(const float startTime)
{
	totalTime = startTime;
	then = std::chrono::system_clock::now();
}

void Timer::updateTime()
{
	if (totalTime > 0)
	{
		now = std::chrono::system_clock::now();
		const auto duration = now - then;
		const auto timeSpanSeconds = std::chrono::duration_cast<std::chrono::duration<int>>(duration);
		if (timeSpanSeconds.count() >= 1)
		{
			totalTime--;
			then = now;
		}
	}
}