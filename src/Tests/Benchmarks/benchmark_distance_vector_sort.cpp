#include <benchmark/benchmark.h>

#include <algorithm>

#include <Math/vector.h>
#include <Math/math.h>


const long fromRange = 2;
const long toRange = 1 << 15;


poke::math::Vec3 GenerateRandomVector()
{
	return poke::math::Vec3(poke::math::Random::Get().RandomRange(0, 100), poke::math::Random::Get().RandomRange(0, 100), poke::math::Random::Get().RandomRange(0, 100));
}

static void BM_EmplaceBackSizeT(benchmark::State& state) {
	std::vector<poke::math::Vec3> position;
	position.reserve(state.range(0));
	for (size_t i = 0; i < state.range(0); i++) {
		position.emplace_back(GenerateRandomVector());
	}
	std::vector<std::pair<float, size_t>> sortedIndex;
	sortedIndex.reserve(state.range(0));

	const poke::math::Vec3 camPos(10, 5, 2);

	for (auto _ : state) {
		for (size_t i = 0; i < state.range(0); i++) {
			const auto distance = poke::math::Vec3::GetDistanceManhattan(camPos, position[i]);
			sortedIndex.emplace_back(std::pair<float, size_t>(distance, i));
		}
		std::sort(sortedIndex.begin(), sortedIndex.end(), [](const std::pair<float, size_t>& d1, const std::pair<float, size_t>& d2) {return d1 < d2; });
	}
}
BENCHMARK(BM_EmplaceBackSizeT)->Range(fromRange, toRange);

static void BM_IndexAccessSizeT(benchmark::State& state) {
	std::vector<poke::math::Vec3> position;
	position.reserve(state.range(0));
	for (size_t i = 0; i < state.range(0); i++) {
		position.emplace_back(GenerateRandomVector());
	}
	std::vector<std::pair<float, size_t>> sortedIndex;
	sortedIndex.resize(state.range(0));

	const poke::math::Vec3 camPos(10, 5, 2);

	for (auto _ : state) {
		for (size_t i = 0; i < state.range(0); i++) {
			const auto distance = poke::math::Vec3::GetDistanceManhattan(camPos, position[i]);
			sortedIndex[i] = std::pair<float, size_t>(distance, i);
		}
		std::sort(sortedIndex.begin(), sortedIndex.end(), [](const std::pair<float, size_t>& d1, const std::pair<float, size_t>& d2) {return d1 < d2; });
	}
}
BENCHMARK(BM_IndexAccessSizeT)->Range(fromRange, toRange);

static void BM_IndexAccessIntRef(benchmark::State& state) {
	std::vector<poke::math::Vec3> position;
	position.reserve(state.range(0));
	for (size_t i = 0; i < state.range(0); i++) {
		position.emplace_back(GenerateRandomVector());
	}
	std::vector<std::pair<float, int>> sortedIndex;
	sortedIndex.resize(state.range(0));

	const poke::math::Vec3 camPos(10, 5, 2);

	for (auto _ : state) {
		for (size_t i = 0; i < state.range(0); i++) {
			const auto distance = poke::math::Vec3::GetDistanceManhattan(camPos, position[i]);
			sortedIndex[i] = std::pair<float, int>(distance, i);
		}
		std::sort(sortedIndex.begin(), sortedIndex.end(), [](const std::pair<float, int>& d1, const std::pair<float, int>& d2) {return d1 < d2; });
	}
}
BENCHMARK(BM_IndexAccessIntRef)->Range(fromRange, toRange);

static void BM_IndexAccessIntNoRef(benchmark::State& state) {
	std::vector<poke::math::Vec3> position;
	position.reserve(state.range(0));
	for (size_t i = 0; i < state.range(0); i++) {
		position.emplace_back(GenerateRandomVector());
	}
	std::vector<std::pair<float, int>> sortedIndex;
	sortedIndex.resize(state.range(0));

	const poke::math::Vec3 camPos(10, 5, 2);

	for (auto _ : state) {
		for (size_t i = 0; i < state.range(0); i++) {
			const auto distance = poke::math::Vec3::GetDistanceManhattan(camPos, position[i]);
			sortedIndex[i] = std::pair<float, int>(distance, i);
		}
		std::sort(sortedIndex.begin(), sortedIndex.end(), [](const std::pair<float, int> d1, const std::pair<float, int> d2) {return d1 < d2; });
	}
}
BENCHMARK(BM_IndexAccessIntNoRef)->Range(fromRange, toRange);

static void BM_IndexAccessIntRefNoConstructor(benchmark::State& state) {
	std::vector<poke::math::Vec3> position;
	position.reserve(state.range(0));
	for (size_t i = 0; i < state.range(0); i++) {
		position.emplace_back(GenerateRandomVector());
	}
	std::vector<std::pair<float, unsigned int>> sortedIndex;
	sortedIndex.resize(state.range(0));

	const poke::math::Vec3 camPos(10, 5, 2);

	for (auto _ : state) {
		for (size_t i = 0; i < state.range(0); i++) {
			sortedIndex[i].first = poke::math::Vec3::GetDistanceManhattan(camPos, position[i]);
			sortedIndex[i].second = i;
		}
		std::sort(sortedIndex.begin(), sortedIndex.end(), [](const std::pair<float, int>& d1, const std::pair<float, int>& d2) {return d1 < d2; });
	}
}
BENCHMARK(BM_IndexAccessIntRefNoConstructor)->Range(fromRange, toRange);
BENCHMARK_MAIN();