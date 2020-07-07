#include <benchmark/benchmark.h>

#include <random>

#include <Ecs/Utility/entity_vector.h>

const long fromRange = 2;
const long toRange = 1 << 15;

float CopyFromShuffledToEntityVector(poke::ecs::EntityVector& v, const std::vector<int>& shuffled)
{
	for (int i : shuffled) {
		v.insert(i);
	}

	return 0;
}

float CopyFromShuffledToVector(std::vector<int>& v, const std::vector<int>& shuffled)
{
	for (int i : shuffled) {
		v.push_back(i);
		std::sort(v.begin(), v.end());
	}

	return 0;
}

float InsertEntity(poke::ecs::EntityVector& v, const int entityIndex)
{
	v.insert(entityIndex);

	return 0;
}

float InsertEntity(std::vector<int>& v, const int entityIndex)
{
	v.push_back(entityIndex);
	std::sort(v.begin(), v.end());

	return 0;
}

static void BM_AddEntityVector(benchmark::State& state) {
	poke::ecs::EntityVector entities(state.range(0));
	std::vector<int> shuffled;
	shuffled.resize(state.range(0));
    for(int i = 0; i < state.range(0); i++) {
		shuffled[i] = i;
    }
	std::random_device rd;
	std::mt19937 g(rd());
	std::shuffle(shuffled.begin(), shuffled.end(), g);
	for (auto _ : state)
		benchmark::DoNotOptimize(CopyFromShuffledToEntityVector(entities, shuffled));
}
BENCHMARK(BM_AddEntityVector)->Range(fromRange, toRange);

static void BM_AddVector(benchmark::State& state) {
	std::vector<int> entities;
	entities.reserve(state.range(0));
	std::vector<int> shuffled;
	shuffled.resize(state.range(0));
	for (int i = 0; i < state.range(0); i++) {
		shuffled[i] = i;
	}
	std::random_device rd;
	std::mt19937 g(rd());
	std::shuffle(shuffled.begin(), shuffled.end(), g);
	for (auto _ : state)
		benchmark::DoNotOptimize(CopyFromShuffledToVector(entities, shuffled));
}
BENCHMARK(BM_AddVector)->Range(fromRange, 1 << 8);

static void BM_AddSortedEntityVector(benchmark::State& state) {
	poke::ecs::EntityVector entities(state.range(0));
	for(int i = 0; i < entities.size() - 1; i++) {
		entities.insert(i);
	}
	for (auto _ : state)
		benchmark::DoNotOptimize(InsertEntity(entities, state.range(0)));
}
BENCHMARK(BM_AddSortedEntityVector)->Range(fromRange, toRange);

static void BM_AddSortedVector(benchmark::State& state) {
	std::vector<int> shuffled;
	shuffled.reserve(state.range(0));
	shuffled.resize(shuffled.capacity());
	for (int i = 0; i < shuffled.size(); i++) {
		shuffled[i] = i;
	}
	for (auto _ : state)
		benchmark::DoNotOptimize(InsertEntity(shuffled, state.range(0)));
}
BENCHMARK(BM_AddSortedVector)->Range(fromRange, toRange);

static void BM_ExistEntityVector(benchmark::State& state) {
	poke::ecs::EntityVector entities(state.range(0));
	for (int i = 0; i < entities.size(); i++) {
		entities.insert(i);
	}
	for (auto _ : state)
		benchmark::DoNotOptimize(entities.exist(state.range(0) / 2));
}
BENCHMARK(BM_ExistEntityVector)->Range(fromRange, toRange);

static void BM_ExistVector(benchmark::State& state) {
	std::vector<int> shuffled;
	shuffled.resize(state.range(0));
	for (int i = 0; i < shuffled.size(); i++) {
		shuffled[i] = i;
	}
	for (auto _ : state) {
		auto it = std::find(shuffled.begin(), shuffled.end(), state.range(0) / 2);
		bool found = false;
		benchmark::DoNotOptimize(found = it != shuffled.end());
	}
}
BENCHMARK(BM_ExistVector)->Range(fromRange, toRange);

static void BM_FindEntityVector(benchmark::State& state) {
	poke::ecs::EntityVector entities(state.range(0));
	for (int i = 0; i < entities.size(); i++) {
		entities.insert(i);
	}
	for (auto _ : state)
		benchmark::DoNotOptimize(entities.exist(state.range(0) / 2));
		benchmark::DoNotOptimize(entities.find(state.range(0) / 2));
}
BENCHMARK(BM_FindEntityVector)->Range(fromRange, toRange);

static void BM_FindVector(benchmark::State& state) {
	std::vector<int> shuffled;
	shuffled.resize(state.range(0));
	for (int i = 0; i < shuffled.size(); i++) {
		shuffled[i] = i;
	}
	for (auto _ : state) {
		benchmark::DoNotOptimize(std::find(shuffled.begin(), shuffled.end(), state.range(0) / 2));
	}
}
BENCHMARK(BM_FindVector)->Range(fromRange, toRange);