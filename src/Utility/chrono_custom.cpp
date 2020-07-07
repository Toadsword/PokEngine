#include <Utility/chronometer.h>

namespace poke
{
Chronometer::Chronometer()
{
	startTime_ = std::chrono::high_resolution_clock::now();
}

void Chronometer::Restart()
{
	startTime_ = std::chrono::high_resolution_clock::now();
}

double Chronometer::GetTime() const
{
	const std::chrono::duration<double, std::nano> elapsedTime = std::chrono::high_resolution_clock::now() -
		startTime_;
	return elapsedTime.count();
}

double Chronometer::GetTimeMicro() const
{
	const std::chrono::duration<double, std::micro> elapsedTime = std::chrono::high_resolution_clock::now() -
		startTime_;
	return elapsedTime.count();
}

double Chronometer::GetTimeMilli() const
{
	const std::chrono::duration<double, std::milli> elapsedTime = std::chrono::high_resolution_clock::now() -
		startTime_;
	return elapsedTime.count();
}
}
