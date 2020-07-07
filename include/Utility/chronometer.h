#pragma once

#include <chrono>

namespace poke
{
/**
 * \brief Use to compute time in nano seconds. Mainly for short code for computing short amount of time and doing some optimization
 */
class Chronometer
{
public:
	/**
	 * \brief Create and start the chronometer immediately
	 */
	Chronometer();
	~Chronometer() = default;

	/**
	 * \brief Reset starting time to now
	 */
	void Restart();

	/**
	 * \brief Return time elapsed in nano seconds
	 */
	double GetTime() const;

	/**
	 * \brief Return time elapsed in micro seconds
	 * \return time in micro seconds
	 */
	double GetTimeMicro() const;

	/**
	 * \brief Return time elapsed in milli seconds
	 * \return time in milli seconds
	 */
	double GetTimeMilli() const;

private:
	std::chrono::steady_clock::time_point startTime_;
};
} //namespace poke
