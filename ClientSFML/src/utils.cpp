#include "utils.h"

#include <random>
#include <ctime>

namespace
{
	std::default_random_engine createRandomEngine()
	{
		auto seed = static_cast<unsigned int>(std::time(nullptr));
		return std::default_random_engine(seed);
	}

	auto RandomEngine = createRandomEngine();
}

namespace rando
{
	int RandomRange(int32 min, int32 max)
	{
		std::uniform_int_distribution<> distr(min, max - 1);
		return distr(RandomEngine);
	}

	float RandFloat(float min, float max)
	{
		std::uniform_real_distribution<float> distr(min, max - 1.0f);
		return distr(RandomEngine);
	}
}
