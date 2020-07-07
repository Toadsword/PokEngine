#include <Utility/timer.h>

namespace poke
{
Timer::Timer(const double durationInS, const bool repeat) :
	duration_(durationInS),
	repeat_(repeat)
{
	startTime_ = std::chrono::high_resolution_clock::now();
}

bool Timer::IsOver()
{
	const auto now = std::chrono::high_resolution_clock::now();
	const std::chrono::duration<double> elapsedTime = now - startTime_;
	
	if (elapsedTime.count() >= duration_ && !isOver_)
	{
		if (repeat_)
		{
			startTime_ = now;
		}
		else
		{
			isOver_ = true;
		}
		return true;
	}

	return false;
}

void Timer::Stop()
{
	isOver_ = true;
}

void Timer::Restart()
{
	isOver_ = false;
	startTime_ = std::chrono::high_resolution_clock::now();
}
} //namespace poke
