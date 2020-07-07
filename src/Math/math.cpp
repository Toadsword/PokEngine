#include <Math/math.h>

poke::math::Random& poke::math::Random::Get() {
	static Random random;
	return random;
}

float poke::math::Random::RandomRange(const float minimum, const float maximum) {
    static std::mt19937 generator(randomDevice_());

    if (seed_ != kNoSeed) {
        generator.seed(seed_ + iteration_);

        iteration_++;
        if (seed_ + iteration_ == kNoSeed) { iteration_++; }
    }

    std::uniform_real_distribution<float> distribution(0, maximum - minimum);

    return distribution(generator) + minimum;
}

int poke::math::Random::RandomRange(const int minimum, const int maximum) {
    static std::mt19937 generator(randomDevice_());

    if (seed_ != kNoSeed) {
        generator.seed(seed_ + iteration_);

        iteration_++;
        if (seed_ + iteration_ == kNoSeed) { iteration_++; }
    }

    std::uniform_int_distribution<int> distribution(0, maximum - minimum);

    return distribution(generator) + minimum;
}

float poke::math::Random::RandomRange(const float maximum) {
	static std::mt19937 generator(randomDevice_());

	if (seed_ != kNoSeed) {
		generator.seed(seed_ + iteration_);

		iteration_++;
		if (seed_ + iteration_ == kNoSeed) { iteration_++; }
	}

	std::uniform_real_distribution<float> distribution(0, maximum);

	return distribution(generator);
}

int poke::math::Random::RandomRange(const int maximum) {
	static std::mt19937 generator(randomDevice_());

	if (seed_ != kNoSeed) {
		generator.seed(seed_ + iteration_);

		iteration_++;
		if (seed_ + iteration_ == kNoSeed) { iteration_++; }
	}

	std::uniform_int_distribution<int> distribution(0, maximum);

	return distribution(generator);
}

void poke::math::Random::SetSeedForRandom(const int newSeed)
{
    seed_ = newSeed;
}
