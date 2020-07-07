#include <Utility/time_custom.h>
#include <iostream>

namespace poke
{
void Time::StartFrame()
{
	startFrame_ = std::chrono::high_resolution_clock::now();
}

void Time::EndFrame()
{
	endFrame_ = std::chrono::high_resolution_clock::now();
	deltaTime = endFrame_ - startFrame_;
}

void Time::ResetFrame()
{
	startFrame_ = std::chrono::high_resolution_clock::now();
	endFrame_ = std::chrono::high_resolution_clock::now();
	deltaTime = std::chrono::duration<double, std::milli>(16.666);
}

float Time::GetFrameElapsedTime() const
{
	const std::chrono::duration<double, std::milli> elapsedTime = std::chrono::high_resolution_clock::now() -
		startFrame_;
	return elapsedTime.count();
}


void Time::GetActualDate(int &year, int &month, int &day, int &hour, int &mins, int &secs)
{
	time_t tt;
	time(&tt);
	tm TM;

	localtime_s(&TM, &tt);
	year = TM.tm_year + 1900;
	month = TM.tm_mon + 1;
	day = TM.tm_mday;
	hour = TM.tm_hour;
	mins = TM.tm_min;
	secs = TM.tm_sec;
}

void Time::StartRecordingTime()
{
	startTime_ = std::chrono::high_resolution_clock::now();
}

Time& Time::Get()
{
	static Time instance;
	return instance;
}

float Time::GetTime() const
{
	const std::chrono::duration<double> elapsedTime = std::chrono::high_resolution_clock::now() -
		startTime_;
	return elapsedTime.count();
}

float Time::GetTimeMilli() const
{
	const std::chrono::duration<double, std::milli> elapsedTime = std::chrono::high_resolution_clock::now() -
		startTime_;
	return elapsedTime.count();
}

} //namespace poke
