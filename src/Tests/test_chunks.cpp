#include <gtest/gtest.h>

#include <CoreEngine/engine.h>
#include <Editor/editor.h>
#include <GraphicsEngine/Renderers/renderer_editor.h>
#include <Utility/file_system.h>
#include <Tests/TestNico/test_system.h>
#include <Tests/TestNico/test_spline.h>
#include <CoreEngine/ServiceLocator/service_locator_definition.h>
#include "Utility/log.h"

class EmptyChunkTest {
public:
	EmptyChunkTest(poke::Engine& engine) :
        engine_(engine)
    {
        
    }

	void OnUpdate()
	{

        
		const int maxEngineFrameNumber = 100;
		count_++;

		if (count_ == maxEngineFrameNumber) {
			engine_.Stop();
		}
	}
private:
	int count_ = 0;

	poke::Engine& engine_;
};

TEST(Chunks, Emptychunks)
{
	poke::EngineSetting engineSettings{
		"testChunks",
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

	EmptyChunkTest emptyChunkTest(engine);
	engine.AddObserver(poke::observer::MainLoopSubject::UPDATE, [&]() {emptyChunkTest.OnUpdate(); });
    //

	engine.Init();

	engine.Run();
}

class ChunkAABB {
public:
	ChunkAABB(poke::Engine& engine) :
		engine_(engine)
	{

	}

    void OnInit()
	{
		using namespace poke;

		auto& ecsManager = EcsManagerLocator::Get();

		TextureManagerLocator::Get().AddTexture2D("Metal29_disp.jpg");
		std::string materialName = "metal_test";
		auto materialHash = XXH64(
			&materialName[0],
			materialName.size(),
			math::kHashSeed);
		MaterialsManagerLocator::Get().AddMaterial(materialName);

		auto& modelsManager =
			ecsManager.GetComponentsManager<ecs::ModelsManager>(
            );

		auto& transformsManager =
			ecsManager.GetComponentsManager<ecs::TransformsManager>(
            );

		auto& collidersManager =
			ecsManager.GetComponentsManager<ecs::CollidersManager>(
            );

		auto& rigidbodiesManager =
			ecsManager.GetComponentsManager<ecs::RigidbodyManager>(
            );

		//Entity 0
		{
			entity0_ = ecsManager.AddEntity();

			ecsManager.AddComponent(entity0_, ecs::ComponentType::MODEL);

			const graphics::Model model(
				materialHash,
				MeshManagerLocator::Get().GetSphereID());
			modelsManager.SetComponent(entity0_, model);

			ecsManager.AddComponent(entity0_, ecs::ComponentType::TRANSFORM);
			auto transform = transformsManager.GetComponent(entity0_);
			transform.SetLocalPosition(math::Vec3(1, 5, 6));
			transformsManager.SetComponent(entity0_, transform);

			//Rigidbody objects
			ecsManager.AddComponent(entity0_, ecs::ComponentType::RIGIDBODY);
			physics::Rigidbody rigidbody;
			rigidbody.type = physics::RigidbodyType::DYNAMIC;
			rigidbodiesManager.SetComponent(entity0_, rigidbody);

            
			const physics::BoxShape boxShape(
				{ 0.0f, 0.0f, 0.0f },
				{ 1.0f, 1.0f, 100.0f });

			ecsManager.AddComponent(entity0_, ecs::ComponentType::COLLIDER);
			physics::Collider collider;
			collider.isTrigger = false;
			collider.SetShape(boxShape);
			collidersManager.SetComponent(entity0_, collider);
		}
	}

    void OnDraw()
	{
		using namespace poke;

		auto& ecsManager = EcsManagerLocator::Get();

		auto& transformsManager =
			ecsManager.GetComponentsManager<ecs::TransformsManager>(
            );

        const auto worldPosition = transformsManager.GetWorldPosition(entity0_);

		const auto worldRotation = transformsManager.GetWorldRotation(entity0_);

		const auto worldScale = transformsManager.GetWorldScale(entity0_);

		auto& collidersManager =
			ecsManager.GetComponentsManager<ecs::CollidersManager>(
            );
		auto shape = collidersManager.GetComponent(entity0_).boxShape;

        const auto aabb = collidersManager.GetComponent(entity0_).ComputeAABB(
			{ worldPosition.x, worldPosition.y, worldPosition.z },
			{ worldScale.x, worldScale.y, worldScale.z },
			{ worldRotation.x, worldRotation.y, worldRotation.z });

	    //Draw collider
		GraphicsEngineLocator::Get().GetGizmoCommandBuffer().SetColor(Color::red);
		GraphicsEngineLocator::Get().GetGizmoCommandBuffer().DrawWireCube(
			worldPosition + shape.GetCenter(),
			{ shape.GetExtent().x * worldScale.x , shape.GetExtent().y * worldScale.y, shape.GetExtent().z * worldScale.z },
			{ worldRotation.x, worldRotation.y, worldRotation.z });

        //Draw aabb
		GraphicsEngineLocator::Get().GetGizmoCommandBuffer().SetColor(Color::white);
		GraphicsEngineLocator::Get().GetGizmoCommandBuffer().DrawWireCube(
			aabb.worldPosition,
			aabb.worldExtent,
			{ 0.0f, 0.0f, 0.0f });
	}

	void OnUpdate()
	{

        if(engine_.GetModuleManager().inputManager.IsKeyDown(poke::input::KeyCode::K)) {
			auto activeChunk = poke::ChunksManagerLocator::Get().GetActiveChunkIndex();
			poke::ChunksManagerLocator::Get().SetActiveChunk(++activeChunk);
        }
	}
private:

	poke::Engine& engine_;

	poke::ecs::EntityIndex entity0_;
};

TEST(Chunks, AABB)
{
	poke::EngineSetting engineSettings{
		"testChunksAABB",
		poke::AppType::EDITOR,
		std::chrono::duration<double, std::milli>(16.66f),
		720,
		640,
		"POK engine",
		{{0, "Default", "TestChunk"}}
	};

	poke::Engine engine(engineSettings);

	//Load editor application
	engine.SetApp(std::make_unique<poke::editor::Editor>(engine, ""));

	//Load editor graphics renderer
	engine.GetModuleManager().graphicsEngine.SetRenderer(
		std::make_unique<poke::graphics::RendererEditor>(engine));

	// TEST

	ChunkAABB emptyChunkTest(engine);
	engine.AddObserver(
        poke::observer::MainLoopSubject::APP_INIT,
        [&]() { emptyChunkTest.OnInit(); });
	engine.AddObserver(poke::observer::MainLoopSubject::UPDATE, [&]() {emptyChunkTest.OnUpdate(); });
	engine.AddObserver(poke::observer::MainLoopSubject::DRAW, [&]() {emptyChunkTest.OnDraw(); });
	//

	engine.Init();

	engine.Run();
}