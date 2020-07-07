#include <benchmark/benchmark.h>

#include <random>

#include <Math/tranform.h>
#include <Memory/vector_view.h>

const long fromRange = 2;
const long toRange = 1 << 15;

class TransformManager {
public:
    TransformManager(size_t size)
    {
		transforms_.resize(size);
    }

    poke::math::Transform& GetComponent(const size_t size)
    {
		return transforms_[size];
    }

	void SetComponent(const size_t size, poke::math::Transform t)
	{
		transforms_[size] = t;
	}

    poke::VectorView<std::vector<poke::math::Transform>::const_iterator> GetTransformsView() const
	{
		return poke::VectorView<std::vector<poke::math::Transform>::const_iterator>(transforms_.begin(), transforms_.end());
	}

	poke::VectorView<std::vector<poke::math::Transform>::iterator> GetTransformsView()
	{
		return poke::VectorView<std::vector<poke::math::Transform>::iterator>(transforms_.begin(), transforms_.end());
	}
private:
	std::vector<poke::math::Transform> transforms_;
};

float CopyComponent(TransformManager& transformManager, std::vector<poke::math::Transform>& v, const int entityIndex)
{
	v[entityIndex] = transformManager.GetComponent(entityIndex);
	return 0;
}

float VectorViewComponent(poke::VectorView<std::vector<poke::math::Transform>::iterator>& vectorView, std::vector<poke::math::Transform>& v, const int entityIndex)
{
	v[entityIndex] = vectorView[entityIndex];
	return 0;
}

static void BM_GetComponent(benchmark::State& state) {
	TransformManager transformManager(state.range(0));
	std::vector<poke::math::Transform> transforms;
	transforms.resize(state.range(0));
	for (auto _ : state) {
		for (auto i = 0; i < state.range(0); i++)
			transforms[i] = transformManager.GetComponent(i);
	}
}
BENCHMARK(BM_GetComponent)->Range(fromRange, toRange);

static void BM_VV_GetComponent(benchmark::State& state) {
	TransformManager transformManager(state.range(0));
	auto vectorView = transformManager.GetTransformsView();
	std::vector<poke::math::Transform> transforms;
	transforms.resize(state.range(0));
	for (auto _ : state) {
		for (auto i = 0; i < state.range(0); i++)
			transforms[i] = vectorView[i];
	}
}
BENCHMARK(BM_VV_GetComponent)->Range(fromRange, toRange);

static void BM_GetSetComponent(benchmark::State& state) {
	TransformManager transformManager(state.range(0));
	for (auto _ : state) {
		for (auto i = 0; i < state.range(0); i++) {
			auto& transform = transformManager.GetComponent(i);
			transform.SetLocalPosition({ 10, 10, 10 });
		}
	}
}
BENCHMARK(BM_GetSetComponent)->Range(fromRange, toRange);

static void BM_VV_GetSetComponent(benchmark::State& state) {
	TransformManager transformManager(state.range(0));
	poke::VectorView<std::vector<poke::math::Transform>::iterator> vectorView = transformManager.GetTransformsView();
	for (auto _ : state) {
		for (auto i = 0; i < state.range(0); i++)
			vectorView[i].SetLocalPosition({ 10, 10, 10 });
	}
}
BENCHMARK(BM_VV_GetSetComponent)->Range(fromRange, toRange);