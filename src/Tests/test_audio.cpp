#include <gtest/gtest.h>

#include <CoreEngine/engine.h>
#include <Editor/editor.h>
#include <GraphicsEngine/Renderers/renderer_editor.h>

class TestHelper {
public:
	TestHelper(poke::Engine& engine, const int maxCounter) : maxCounter_(maxCounter), engine_(engine){}

protected:
	void UpdateCounter()
	{
		count_++;

        if(count_ == maxCounter_) {
			engine_.Stop();
        }
	}

private:
	int count_ = 0;
	int maxCounter_ = 10;

	poke::Engine& engine_;
};

class PlayOnStart : public TestHelper{
public:
	PlayOnStart(poke::Engine& engine, const int maxCount) :
		TestHelper(engine, maxCount)
    {
        
    }

    void OnAppInit()
	{
		using namespace poke;
		const std::string soundPath = PokFileSystem::GetFullPath(
			"droplet.wav",
			FileType::SOUNDS,
			FolderType::ROM);
		const auto soundHash = XXH64(&soundPath[0], soundPath.size(), math::kHashSeed);
		SoundsManagerLocator::Get().AddSound(soundPath);
		auto& ecsManager = EcsManagerLocator::Get();

		auto& transformManager = ecsManager.GetComponentsManager<ecs::TransformsManager>();
		auto& audioSourceManager = ecsManager.GetComponentsManager<ecs::AudioSourcesManager>();

		// Main object
		{
            const auto entityIndex = ecsManager.AddEntity();

            //Transform
			ecsManager.AddComponent(entityIndex, ecs::ComponentType::TRANSFORM);
			auto transform = transformManager.GetComponent(entityIndex);
			transform.SetLocalPosition(math::Vec3(0, 0, 0));
			transformManager.SetComponent(entityIndex, transform);

            //Audio source
			audio::AudioSource audioSource;
			//audioSource.frequency = 44100;
			audioSource.SetPitch(1);
			//audioSource.minDistance = 0;
			//audioSource.maxDistance = 10;
			audioSource.playOnLoadScene = true;
			audioSource.SetVolume(50);
			audioSource.soundID = soundHash;
			audioSourceManager.SetComponent(entityIndex, audioSource);
			ecsManager.AddComponent(entityIndex, ecs::ComponentType::AUDIO_SOURCE);
		}
	}

    void OnUpdate()
    {
		UpdateCounter();

        if(poke::AudioEngineLocator::Get().IsPlaying(0)) {
			condition1_ = true;
        }
    }

    bool HasSucceed()
	{
		return condition1_;
	}

private:
	bool condition1_;
};

TEST(Audio, TestPlayOnStart)
{
	poke::EngineSetting engineSettings{
		"testAudioPlayOnStart",
		poke::AppType::EDITOR,
		std::chrono::duration<double, std::milli>(16.66f),
		720,
		640,
		"POK engine",
		{{0, "Default", "Default"}}
	};

	poke::Engine engine(engineSettings);

	//Load editor application
	engine.SetApp(std::make_unique<poke::editor::Editor>(engine, ""));

	//Load editor graphics renderer
	engine.GetModuleManager().graphicsEngine.SetRenderer(
		std::make_unique<poke::graphics::RendererEditor>(engine));

	// TEST

	PlayOnStart testModule(engine, 10);

	engine.AddObserver(poke::observer::MainLoopSubject::APP_INIT, [&]() {testModule.OnAppInit(); });
	engine.AddObserver(poke::observer::MainLoopSubject::UPDATE, [&]() {testModule.OnUpdate(); });

	// TEST

	engine.Init();



	engine.Run();

	ASSERT_TRUE(testModule.HasSucceed());
}

class PlayManySongs : public TestHelper {
public:
	PlayManySongs(poke::Engine& engine, const int maxCount) :
		TestHelper(engine, maxCount)
	{

	}

	void OnAppInit()
	{
		using namespace poke;
		const std::string soundPath = PokFileSystem::GetFullPath(
			"droplet.wav",
			FileType::SOUNDS,
			FolderType::ROM);
		const auto soundHash = XXH64(&soundPath[0], soundPath.size(), math::kHashSeed);
		SoundsManagerLocator::Get().AddSound(soundPath);
		auto& ecsManager = EcsManagerLocator::Get();

		auto& transformManager = ecsManager.GetComponentsManager<ecs::TransformsManager>();
		auto& audioSourceManager = ecsManager.GetComponentsManager<ecs::AudioSourcesManager>();

		// Main object
		for(int i = 0; i < audio::maxChannel * 2; i++){
		    const auto entityIndex = ecsManager.AddEntity();

			//Transform
			ecsManager.AddComponent(entityIndex, ecs::ComponentType::TRANSFORM);
			auto transform = transformManager.GetComponent(entityIndex);
			transform.SetLocalPosition(math::Vec3(0, 0, 0));
			transformManager.SetComponent(entityIndex, transform);

			//Audio source
			audio::AudioSource audioSource;
			//audioSource.frequency = 44100;
			audioSource.SetPitch(1.0f);
			//audioSource.minDistance = 0;
			//audioSource.maxDistance = 10;
			audioSource.playOnLoadScene = true;
			audioSource.SetVolume(50);
			audioSource.soundID = soundHash;
			audioSourceManager.SetComponent(entityIndex, audioSource);
			ecsManager.AddComponent(entityIndex, ecs::ComponentType::AUDIO_SOURCE);
		}
	}

	void OnUpdate()
	{
		UpdateCounter();

		for (int i = 0; i < poke::audio::maxChannel; i++) {
			if (poke::AudioEngineLocator::Get().IsPlaying(0)) {
				condition1_[i] = true;
			}
		}

	}

	bool HasSucceed()
	{
        for (auto condition1 : condition1_) {
			if (!condition1) return false;
		}
		return true;
	}

private:
	std::vector<bool> condition1_ = std::vector<bool>(poke::audio::maxChannel);
};

TEST(Audio, TestTooMuchSongs)
{
	poke::EngineSetting engineSettings{
		"testAudioTestTooMuchSongs",
		poke::AppType::EDITOR,
		std::chrono::duration<double, std::milli>(16.66f),
		720,
		640,
		"POK engine",
		{{0, "Default", "Default"}}
	};

	poke::Engine engine(engineSettings);

	//Load editor application
	engine.SetApp(std::make_unique<poke::editor::Editor>(engine, ""));

	//Load editor graphics renderer
	engine.GetModuleManager().graphicsEngine.SetRenderer(
		std::make_unique<poke::graphics::RendererEditor>(engine));

	// TEST

	PlayManySongs testModule(engine, 10);

	engine.AddObserver(poke::observer::MainLoopSubject::APP_INIT, [&]() {testModule.OnAppInit(); });
	engine.AddObserver(poke::observer::MainLoopSubject::UPDATE, [&]() {testModule.OnUpdate(); });

	// TEST

	engine.Init();
    
	engine.Run();

	ASSERT_TRUE(testModule.HasSucceed());
}

class PlayLoop : public TestHelper {
public:
	PlayLoop(poke::Engine& engine, const int maxCount) :
		TestHelper(engine, maxCount){}

	void OnAppInit()
	{
		using namespace poke;
		const std::string soundPath = PokFileSystem::GetFullPath(
			"droplet.wav",
			FileType::SOUNDS,
			FolderType::ROM);
		const auto soundHash = XXH64(&soundPath[0], soundPath.size(), math::kHashSeed);
		SoundsManagerLocator::Get().AddSound(soundPath);
		auto& ecsManager = EcsManagerLocator::Get();

		auto& transformManager = ecsManager.GetComponentsManager<ecs::TransformsManager>();
		auto& audioSourceManager = ecsManager.GetComponentsManager<ecs::AudioSourcesManager>();

		// Main object
		{
			const auto entityIndex = ecsManager.AddEntity();

			//Transform
			ecsManager.AddComponent(entityIndex, ecs::ComponentType::TRANSFORM);
			auto transform = transformManager.GetComponent(entityIndex);
			transform.SetLocalPosition(math::Vec3(0, 0, 0));
			transformManager.SetComponent(entityIndex, transform);

			//Audio source
			audio::AudioSource audioSource;
			//audioSource.frequency = 44100;
			audioSource.SetPitch(1.0f);
			//audioSource.minDistance = 0;
			//audioSource.maxDistance = 10;
			audioSource.playOnLoadScene = true;
			audioSource.SetVolume(50);
			audioSource.soundID = soundHash;
			audioSourceManager.SetComponent(entityIndex, audioSource);
			ecsManager.AddComponent(entityIndex, ecs::ComponentType::AUDIO_SOURCE);
		}
	}

	void OnUpdate()
	{
		UpdateCounter();

		/*for (int i = 0; i < poke::audio::maxChannel; i++) {
			if (poke::AudioEngine::Get().IsPlaying(0)) {
				condition1_[i] = true;
			}
		}*/

	}

	bool HasSucceed()
	{
		
		return true;
	}

private:
	std::vector<bool> condition1_ = std::vector<bool>(poke::audio::maxChannel);
};

TEST(Audio, TestPlayLoop)
{
	poke::EngineSetting engineSettings{
		"testAudioTestPlayLoop",
		poke::AppType::EDITOR,
		std::chrono::duration<double, std::milli>(16.66f),
		720,
		640,
		"POK engine",
		{{0, "Default", "Default"}}
	};

	poke::Engine engine(engineSettings);

	//Load editor application
	engine.SetApp(std::make_unique<poke::editor::Editor>(engine, ""));

	//Load editor graphics renderer
	engine.GetModuleManager().graphicsEngine.SetRenderer(
		std::make_unique<poke::graphics::RendererEditor>(engine));

	// TEST

	PlayLoop testModule(engine, 100);

	engine.AddObserver(poke::observer::MainLoopSubject::APP_INIT, [&]() {testModule.OnAppInit(); });
	engine.AddObserver(poke::observer::MainLoopSubject::UPDATE, [&]() {testModule.OnUpdate(); });

	// TEST

	engine.Init();

	engine.Run();

	ASSERT_TRUE(testModule.HasSucceed());
}