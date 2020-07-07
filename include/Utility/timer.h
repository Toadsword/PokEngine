#pragma once

#include <chrono>

namespace poke
{
/**
 * \brief Used to compute time for a certain duration in seconds. 
 */
class Timer
{
public:
	/**
	 * \brief Timer in seconds
	 * \param durationInS duration of the timer in seconds
	 * \param repeat indicate if this timer should repeat every time the duration has been covered
	 */
	explicit Timer(double durationInS, bool repeat = false);
	~Timer() = default;

	/**
	 * \brief if the timer is over return true. If it's not on repeat time it will return true only once 
	 * \return true if finish
	 */
	bool IsOver();

	/**
	 * \brief Can stop the timer. If need to be re launch use Restart()
	 */
	void Stop();

	/**
	 * \brief Can restart the timer 
	 */
	void Restart();

private:
	std::chrono::steady_clock::time_point startTime_;

	double duration_ = 0.0;

	bool isOver_ = false;
	bool repeat_ = false;
};
} //namespace poke
