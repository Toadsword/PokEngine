#include <Editor/editor.h>

#include <imgui.h>

#include <misc/cpp/imgui_stdlib.h>

#include <CoreEngine/engine.h>
#include <Utility/file_system.h>
#include <Editor/Tools/component_tool.h>
#include <Editor/Tools/log_tool.h>
#include <Editor/Tools/stats_tool.h>
#include <Editor/Tools/shader_converter_tool.h>
#include <Editor/Tools/localization_tool.h>
#include <Editor/Tools/input_binding_tool.h>
#include <Editor/Tools/prefab_creator_tool.h>
#include <Editor/Tools/particle_tool.h>
#include <Editor/Tools/convertible_scene_to_pokscene_tool.h>
#include <Editor/Tools/example_tool.h>
#include <Editor/Showroom/showroom.h>
#include <Editor/Tools/resources_viewer.h>
#include <Editor/Tools/gizmo_display_tool.h>
#include <Editor/Tools/chunk_editor_tool.h>
#include <Editor/gizmo_utility.h>
#include <Editor/Tools/graphics_controller_tool.h>
#include <CoreEngine/ServiceLocator/service_locator_definition.h>
#include <Utility/profiler.h>
#include <Utility/time_custom.h>

namespace poke {
namespace editor {
Editor::Editor(Engine& engine, const std::string& fileName)
    : EngineApplication(engine, fileName),
      engine_(engine),
      inputManager_(engine.GetModuleManager().inputManager),
      sceneManager_(engine.GetModuleManager().sceneManager),
      scenes_(engine.GetModuleManager().sceneManager.GetScenes()),
      game_(engine, ""),
      editorEcsManager_(engine, game_, engine.GetEngineSettings().GetDefaultPoolSize()),
      gameCameraCopy_(engine)
{
    engine_.AddObserver(
        observer::MainLoopSubject::APP_INIT,
        [this]() { OnAppBuild(); });

    subjectDrawImGui_.AddObserver([this]() { OnDrawImGui(); });
    subjectUpdate_.AddObserver([this]() { OnUpdate(); });
    subjectEndFrame_.AddObserver([this]() { OnEndOfFrame(); });

    game_.Stop();
}

Editor::~Editor()
{
    // Write the to an userprefs file the last tools opened
    ToolFlags toolFlags = 0;
    for (auto& tool : tools_) {
        if (tool->IsActive()) { toolFlags |= tool->GetFlag(); }
        toolFlagsPrefs_ &= 0; // We don't want to register the available 
    }
    toolFlags |= toolFlagsPrefs_;
    // Keep the prefs for unavailable tools inside the variable

    json json;
    json["openedTools"] = toolFlags;

    PokFileSystem::WriteFile(
        "prefs",
        json,
        FileType::USER_PREFS,
        FolderType::SAVE
    );
}

void Editor::Build()
{
    //New re assign
    MaterialsManagerLocator::Assign(
        &resourcesManagerContainer_.editorMaterialsManager);
    TextureManagerLocator::Assign(
        &resourcesManagerContainer_.editorTexturesManager);
    SoundsManagerLocator::Assign(
        &resourcesManagerContainer_.editorSoundsManager);
    MeshManagerLocator::Assign(
        &resourcesManagerContainer_.editorMeshesManager);
    EcsManagerLocator::Assign(&editorEcsManager_);
    PrefabsManagerLocator::Assign(
        &resourcesManagerContainer_.editorPrefabsManager);

	resourcesManagerContainer_.editorMaterialsManager.SetBaseMaterialNames();

	game_.Build();
    CameraLocator::Assign(&camera_);
	
    resourcesManagerContainer_.Init();
}

void Editor::OnAppBuild()
{
    const ToolFlags availableTools =
        engine_.GetEngineSettings().GetAvailableToolsFlag();

    const json json = PokFileSystem::ReadJsonFile(
        "prefs",
        FileType::USER_PREFS,
        FolderType::SAVE
    );

    if (CheckJsonExists(json, "openedTools") &&
        CheckJsonNumber(json, "openedTools")) { toolFlagsPrefs_ = json["openedTools"]; } else {
        toolFlagsPrefs_ = 0;
    }

    tools_.push_back(
        std::make_unique<ExampleTool>(
            *this,
            false)
    );

    ComponentUtility::SetEditor(*this);
	GizmoUtility::SetEditor(*this);
	GizmoUtility::SetGizmoFlags(GIZMO_FLAG_ALL);

    //Create tool lists
    if (availableTools & TOOL_FLAG_STATS) {
        tools_.push_back(
            std::make_unique<StatsTool>(
                *this,
                toolFlagsPrefs_ & TOOL_FLAG_STATS)
        );
    }
    if (availableTools & TOOL_FLAG_LOG) {
        tools_.push_back(
            std::make_unique<LogTool>(
                *this,
                toolFlagsPrefs_ & TOOL_FLAG_LOG)
        );
    }
    if (availableTools & TOOL_FLAG_COMPONENT) {
        tools_.push_back(
            std::make_unique<ComponentTool>(
                *this,
                toolFlagsPrefs_ & TOOL_FLAG_COMPONENT)
        );
    }

    if (availableTools & TOOL_FLAG_CONVERTIBLE_SCENE) {
        tools_.push_back(
            std::make_unique<ConvertibleSceneToPokSceneTool>(
                *this,
                toolFlagsPrefs_ & TOOL_FLAG_CONVERTIBLE_SCENE)
        );
    }

	if (availableTools & TOOL_FLAG_GRAPHIC_CONTROLLER) {
		tools_.push_back(
			std::make_unique<GraphicControllerTool>(
				*this,
				toolFlagsPrefs_ & TOOL_FLAG_GRAPHIC_CONTROLLER)
		);
	}
	if (availableTools & TOOL_FLAG_CHUNK_EDITOR) {
		tools_.push_back(
			std::make_unique<ChunkEditorTool>(
				*this,
				toolFlagsPrefs_ & TOOL_FLAG_CHUNK_EDITOR)
		);
	}
	if (availableTools & TOOL_FLAG_RESOURCES_VIEWER) {
		tools_.push_back(
			std::make_unique<ResourcesViewerTool>(
				*this,
				false)
		);
	}

	if (availableTools & TOOL_FLAG_PARTICLE) {
		tools_.push_back(
			std::make_unique<ParticleTool>(
				*this,
				toolFlagsPrefs_ & TOOL_FLAG_PARTICLE)
		);
	}
	if (availableTools & TOOL_FLAG_GIZMO_DISPLAY) {
		tools_.push_back(
			std::make_unique<GizmoDisplayTool>(
				*this,
				false)
		);
	}
    if (availableTools & TOOL_FLAG_LOCALIZATION) {
        tools_.push_back(
            std::make_unique<LocalizationTool>(
                *this,
                toolFlagsPrefs_ & TOOL_FLAG_LOCALIZATION)
        );
    }
    if (availableTools & TOOL_FLAG_SHADER_CONVERTER) {
        tools_.push_back(
            std::make_unique<ShaderConverterTool>(
                *this,
                toolFlagsPrefs_ & TOOL_FLAG_SHADER_CONVERTER)
        );
    }
    if (availableTools & TOOL_FLAG_INPUT_BINDING) {
        tools_.push_back(
            std::make_unique<InputBindingTool>(
                *this,
                toolFlagsPrefs_ & TOOL_FLAG_INPUT_BINDING)
        );
    }
    if (availableTools & TOOL_FLAG_PREFAB_CREATOR) {
        tools_.push_back(
            std::make_unique<PrefabCreatorTool>(
                *this,
                false)
        );
    }

    if (engine_.GetEngineSettings().GetAppType() == AppType::SHOWROOM) {
        tools_.push_back(std::make_unique<Showroom>(*this, true));
    }

    //Set dock space flags
    dockSpaceFlags_ = ImGuiDockNodeFlags_None |
                      ImGuiDockNodeFlags_PassthruCentralNode;

    //Set window flags
    windowFlags_ = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking |
                   ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                   ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                   ImGuiWindowFlags_NoBringToFrontOnFocus |
                   ImGuiWindowFlags_NoNavFocus;
}

void Editor::OnUpdate()
{
      ImGuiIO& io = ImGui::GetIO();

    //Rotation
	math::Vec3 rotation = camera_.GetRotation();
	if (!io.WantCaptureMouse) {
		if (inputManager_.IsMouseButtonDown(input::MouseButtonCode::RIGHT)) {
			dragPosition_ = inputManager_.GetMousePosition();
		}

		if (inputManager_.IsMouseButtonHeld(input::MouseButtonCode::RIGHT)) {
			const math::Vec3 rotationDelta = (dragPosition_ - inputManager_.GetMousePosition())* 0.005f;
			rotation.x += rotationDelta.y;
			if (rotation.x < -kCameraMaxOrientation_ * math::kDeg2Rad) {
				rotation.x = -kCameraMaxOrientation_ * math::kDeg2Rad;
			}

			if (rotation.x > kCameraMaxOrientation_ * math::kDeg2Rad) {
				rotation.x = kCameraMaxOrientation_ * math::kDeg2Rad;
			}
			rotation.y += rotationDelta.x;
			dragPosition_ = inputManager_.GetMousePosition();
		}
	}

    //Movement
	auto speed = kCameraSpeed_;
	if (!io.WantTextInput) {
		if (inputManager_.IsKeyHeld(input::KeyCode::KEY_LEFT_SHIFT)) {
		    speed *= 2;
		}
	}
	math::Vec3 position = camera_.GetPosition();
	const math::Vec3 front = camera_.GetFront() * speed;
	const math::Vec3 right = math::Vec3::Cross(camera_.GetFront(), { 0, 1, 0 }) * speed;

	if (!io.WantTextInput) {
		if (inputManager_.IsKeyHeld(input::KeyCode::W)) { position -= front; }
		if (inputManager_.IsKeyHeld(input::KeyCode::S)) { position += front; }
		if (inputManager_.IsKeyHeld(input::KeyCode::A)) { position += right; }
		if (inputManager_.IsKeyHeld(input::KeyCode::D)) { position -= right; }
	}

	// Zoom
	if (!io.WantCaptureMouse) {
		position -= camera_.GetFront() * inputManager_.scrollY;
	}

    camera_.SetPosition(position);
    camera_.SetRotation(rotation);
	camera_.Update();
}

void Editor::OnEndOfFrame()
{
    if (mustStartGame_) {
        editorEcsManager_.SaveState();

        mustStartGame_ = false;

        gameCameraCopy_ = *game_.GetCameraPtr();
        CameraLocator::Assign(game_.GetCameraPtr());

        ImGui::StyleColorsClassic();
    }

    if (mustStopGame_) {
        editorEcsManager_.ReloadState();

        mustStopGame_ = false;

        ImGui::StyleColorsDark();

        *game_.GetCameraPtr() = gameCameraCopy_;
        CameraLocator::Assign(&camera_);
    }
}

void Editor::LoadApp()
{
    if (appFileName_.empty()) { return; }

    std::string file = PokFileSystem::ReadFile(appFileName_, FileType::DATA);
}

void Editor::Pause() { state_ = AppState::PAUSE; }

void Editor::Run()
{
    state_ = AppState::RUNNING;

    while (state_ != AppState::STOP) {
        if (state_ == AppState::PAUSE)
            continue;

        const bool gameIsPause = game_.GetState() != AppState::RUNNING;

        std::vector<std::string> loadingTitle = {
            "[=          ]",
            "[ =         ]",
            "[  =        ]",
            "[   =       ]",
            "[    =      ]",
            "[     =     ]",
            "[      =    ]",
            "[       =   ]",
            "[        =  ]",
            "[         = ]",
            "[          =]",
            "[         = ]",
            "[        =  ]",
            "[       =   ]",
            "[      =    ]",
            "[     =     ]",
            "[    =      ]",
            "[   =       ]",
            "[  =        ]",
            "[ =         ]"
        };
        auto& sceneManager = SceneManagerLocator::Get();
        std::string title = sceneManager.GetActiveScene().GetSceneName();

        static int iteration = 0;
        GraphicsEngineLocator::Get().SetTitle(
            (loadingTitle[iteration++ % loadingTitle.size()] + " " + title).
            c_str());

        pok_BeginFrame(0);

        Time::Get().StartFrame();
        subjectInputs_.Notify();
        if (!gameIsPause) { game_.NotifyInput(); }

        //Begin profiling threads

        //Main thread
        //Being profiling main thread
        engine_.AddAsync(
            [] { pok_BeginProfiling(App_Thread, 0) },
            ThreadType::MAIN);

        //PhysicsUpdate
		if (!gameIsPause) {
			engine_.AddAsync(
				[this] {
				    pok_BeginProfiling(Update_Physics, 0);
				    subjectPhysicsUpdate_.Notify();
				    pok_EndProfiling(Update_Physics);
			    },
				ThreadType::MAIN);
		}
        if (!gameIsPause) {
            engine_.AddAsync(
                [this] {
                    pok_BeginProfiling(Update_Physics_Game, 0);
                    game_.NotifyPhysicsUpdate();
                    pok_EndProfiling(Update_Physics_Game);
                },
                ThreadType::MAIN);
        }

        //Update
        engine_.AddAsync(
            [this] {
                pok_BeginProfiling(Update, 0);
                subjectUpdate_.Notify();
                pok_EndProfiling(Update);
            },
            ThreadType::MAIN);
        if (!gameIsPause) {
            engine_.AddAsync(
                [this] {
                    pok_BeginProfiling(Update_Game, 0);
                    game_.NotifyUpdate();
                    pok_EndProfiling(Update_Game);
                },
                ThreadType::MAIN);
        }

        //Draw
        engine_.AddAsync(
            [this] {
                pok_BeginProfiling(Draw, 0);
                subjectDraw_.Notify();
                pok_EndProfiling(Draw);
            },
            ThreadType::MAIN);
        if (!gameIsPause) {
            engine_.AddAsync(
                [this] {
                    pok_BeginProfiling(Draw_Game, 0);
                    game_.NotifyDraw();
                    pok_EndProfiling(Draw_Game);
                },
                ThreadType::MAIN);
        }

        //End profiling main thread
        engine_.AddAsync(
            [] { pok_EndProfiling(App_Thread); },
            ThreadType::MAIN);

        //Draw thread
        //Begin profiling render thread
        engine_.AddAsync(
            [] { pok_BeginProfiling(Render_Thread, 0) },
            ThreadType::RENDER);

        //Draw Imgui
        //TODO(@Nico) reactivate imgui
        engine_.AddAsync(
            [this] {
                pok_BeginProfiling(Draw_ImGui, 0);
                subjectDrawImGui_.Notify();
                pok_EndProfiling(Draw_ImGui);
            },
            ThreadType::RENDER);

        //Culling
        engine_.AddAsync(
            [this] {
                pok_BeginProfiling(Culling, 0);
                subjectCulling_.Notify();
                pok_EndProfiling(Culling);
            },
            ThreadType::RENDER);
        if (!gameIsPause) {
            engine_.AddAsync(
                [this] {
                    pok_BeginProfiling(Culling_Game, 0);
                    game_.NotifyCulling();
                    pok_EndProfiling(Culling_Game);
                },
                ThreadType::RENDER);
        }

        //Render
        engine_.AddAsync(
            [this] {
                pok_BeginProfiling(Render, 0);
                subjectRender_.Notify();
                pok_EndProfiling(Render);
            },
            ThreadType::RENDER);
        if (!gameIsPause) {
            engine_.AddAsync(
                [this] {
                    pok_BeginProfiling(Render_Game, 0);
                    game_.NotifyRender();
                    pok_EndProfiling(Render_Game);
                },
                ThreadType::RENDER);
        }

        //End profiling render thread
        engine_.AddAsync(
            [] { pok_EndProfiling(Render_Thread) },
            ThreadType::RENDER);

        //End profiling threads
        //Wait end of frame
        engine_.Wait(ThreadType::MAIN);
        engine_.Wait(ThreadType::RENDER);
        engine_.Wait(ThreadType::WORKER);

        //End frame
        pok_BeginProfiling(End_Frame, 0);
        subjectEndFrame_.Notify();
        if (!gameIsPause) { game_.NotifyEndFrame(); }
        pok_EndProfiling(End_Frame);

        //There is more constancy doing this than doing a Sleep().
        while (Time::Get().GetFrameElapsedTime() < 16.65f) {}

        Time::Get().EndFrame();

        pok_EndFrame();
    }
}

void Editor::Stop() { state_ = AppState::STOP; }

ResourcesManagerContainer& Editor::GetResourcesManagerContainer()
{
    return resourcesManagerContainer_;
}

EditorEcsManager& Editor::GetEditorEcsManager() { return editorEcsManager_; }

void Editor::StartGame()
{
    mustStartGame_ = true;
    game_.UnPause();
}

void Editor::StopGame()
{
    mustStopGame_ = true;
    game_.Stop();
}

void Editor::PauseGame() { game_.Pause(); }

void Editor::RegisterObserverUpdate(const std::function<void()> callback)
{
    subjectUpdate_.AddObserver(callback);
}

void Editor::RegisterObserverPhysicsUpdate(const std::function<void()> callback)
{
    subjectPhysicsUpdate_.AddObserver(callback);
}

void Editor::RegisterObserverDrawImGui(const std::function<void()> callback)
{
    subjectDrawImGui_.AddObserver(callback);
}

void Editor::RegisterObserverEndFrame(const std::function<void()> callback)
{
    subjectEndFrame_.AddObserver(callback);
}

void Editor::RegisterObserverRender(const std::function<void()> callback)
{
    subjectRender_.AddObserver(callback);
}

void Editor::RegisterObserverDraw(const std::function<void()> callback)
{
    subjectDraw_.AddObserver(callback);
}

void Editor::RegisterObserverCulling(const std::function<void()> callback)
{
    subjectCulling_.AddObserver(callback);
}

void Editor::RegisterObserverInput(const std::function<void()> callback)
{
    subjectInputs_.AddObserver(callback);
}

void Editor::OnDrawImGui()
{
	static bool skipFrame = false;
    if(!skipFrame) {
		skipFrame = true;
		return;
    }

	//Main ImGui window's style
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

	//Dockspace flags
	if (dockSpaceFlags_ & ImGuiDockNodeFlags_PassthruCentralNode)
		windowFlags_ |= ImGuiWindowFlags_NoBackground;

	ImGui::Begin("Pok Editor", &showMainWindow_, windowFlags_);
	ImGui::PopStyleVar(3); //Remove all style

	// DockSpace
	ImGuiIO& io = ImGui::GetIO();

	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
		const ImGuiID dockSpaceId = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockSpaceId, ImVec2(0.0f, 0.0f), dockSpaceFlags_);
	}

	//ImGui can't open modal in a MenuBar
	bool openModalSaveScene = false;
	bool openModalDeleteScene = false;

	ResourceAccessorTool::ResetIndexCount();
	//Main bar    
	if (ImGui::BeginMenuBar()) {
		//Docking menu
		if (ImGui::BeginMenu("Docking")) {
			if (ImGui::MenuItem(
				"Flag: NoSplit",
				nullptr,
				(dockSpaceFlags_ & ImGuiDockNodeFlags_NoSplit)
				!= 0)) {
				dockSpaceFlags_ ^= ImGuiDockNodeFlags_NoSplit;
			}
			if (ImGui::MenuItem(
				"Flag: NoResize",
				nullptr,
				(dockSpaceFlags_ & ImGuiDockNodeFlags_NoResize)
				!= 0)) {
				dockSpaceFlags_ ^= ImGuiDockNodeFlags_NoResize;
			}
			if (ImGui::MenuItem(
				"Flag: NoDockingInCentralNode",
				nullptr,
				(dockSpaceFlags_ &
					ImGuiDockNodeFlags_NoDockingInCentralNode) !=
				0)) {
				dockSpaceFlags_ ^= ImGuiDockNodeFlags_NoDockingInCentralNode;
			}
			if (ImGui::MenuItem(
				"Flag: PassthruCentralNode",
				nullptr,
				(dockSpaceFlags_ &
					ImGuiDockNodeFlags_PassthruCentralNode) != 0)
				) {
				dockSpaceFlags_ ^= ImGuiDockNodeFlags_PassthruCentralNode;
			}
			if (ImGui::MenuItem(
				"Flag: AutoHideTabBar",
				nullptr,
				(dockSpaceFlags_ &
					ImGuiDockNodeFlags_AutoHideTabBar) != 0)) {
				dockSpaceFlags_ ^= ImGuiDockNodeFlags_AutoHideTabBar;
			}

			ImGui::EndMenu();
		}

		//Tool list
		if (ImGui::BeginMenu("Tools")) {
			for (auto& tool : tools_) {
				if (ImGui::MenuItem(tool->GetName(), nullptr, tool->IsActive())) {
					tool->SetActive(!tool->IsActive());
				}
			}

			if (ImGui::MenuItem("Demo Tool", nullptr, showDemoWindow_)) {
				showDemoWindow_ = !showDemoWindow_;
			}

			ImGui::EndMenu();
		}

        //Scenes
		if (engine_.GetEngineSettings().GetAppType() != AppType::SHOWROOM) {
			//Scene Option
			if (inputManager_.IsKeyHeld(input::KeyCode::KEY_LEFT_CTRL) &&
				inputManager_.IsKeyDown(input::KeyCode::S)) {
				sceneManager_.SaveActiveScene();
			}

			scenes_ = sceneManager_.GetScenes();

			if (ImGui::BeginMenu("Scenes")) {
				if (ImGui::MenuItem("Add Scene...")) { showAddSceneMenu_ = true; }

				if (ImGui::MenuItem("Save Scene")) { sceneManager_.SaveActiveScene(); }
				if (ImGui::MenuItem("Save Scene as ...")) { openModalSaveScene = true; }
				if (ImGui::BeginMenu("Load Scene...")) {
					const auto activeSceneIndex = sceneManager_.
						GetActiveSceneIndex();
					for (auto& scene : scenes_) {
						const auto sceneIndex = scene.GetSceneIndex();
						if (ImGui::MenuItem(
							(scene.GetSceneName() + "(" + std::to_string(sceneIndex)
								+ ")").c_str(),
							nullptr,
							activeSceneIndex == sceneIndex)) {
								sceneManager_.LoadScene(sceneIndex);
						}
					}
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Remove Scene...")) {
					for (size_t i = 0; i < scenes_.size(); i++) {
						if (ImGui::MenuItem(
							scenes_[i].GetSceneName().c_str(),
							nullptr,
							sceneManager_.GetActiveSceneIndex() == i)) {
							if (sceneManager_.GetActiveSceneIndex() != i) {
								sceneManager_.SaveActiveScene();

								sceneManager_.RemoveScene(scenes_[i].GetSceneIndex());
							}
						}
					}
					ImGui::EndMenu();
				}
				ImGui::EndMenu();
			}

		    if (showAddSceneMenu_) {
				Resource outResource = ResourceAccessorTool::DisplayGetResource(
					"Add Scene...",
					ResourceAccessorTool::DisplayType::DISPLAYED,
					FileType::SCENE,
					FolderType::ROM
				);

				if (ResourceAccessorTool::IsResourceValid(outResource)) {
					// We don't want to add editors scenes manually
					if (outResource.name != "ShowroomScene" && outResource.name !=
						"PrefabCreatorScene") {
						const size_t lastSceneIndex =
							SceneManagerLocator::Get().GetHighestSceneIndex() + 1;
						std::string sceneFolderPath = PokFileSystem::GetPath(
							FileType::SCENE,
							FolderType::ROM);
						size_t nameSize =
							outResource.path.size() - sceneFolderPath.size() - outResource
							.extension.size();

						if (ResourceAccessorTool::IsResourceValid(outResource)) {
							// We don't want to add editors scenes manually
							if (outResource.name != "ShowroomScene" && outResource.name !=
								"PrefabCreatorScene") {
								const size_t lastSceneIndex =
									SceneManagerLocator::Get().GetHighestSceneIndex() + 1;

								std::string scenePath = PokFileSystem::GetSubPathFromFullPath(
									outResource.path,
									FileType::SCENE,
									FolderType::ROM,
									false);

								const scene::Scene newScene = scene::Scene(
									lastSceneIndex,
									outResource.name,
									scenePath);
								SceneManagerLocator::Get().AddScene(newScene);
							}
							else {
								LogDebug(
									"We don't load editor specific scenes into the project !",
									LogType::EDITOR_LOG);
							}
							showAddSceneMenu_ = false;
						}
					}
				}
			}

		    if (openModalSaveScene) { ImGui::OpenPopup("Save Scene as ..."); }

		    if (ImGui::BeginPopupModal(
				"Save Scene as ...",
				nullptr,
				ImGuiWindowFlags_AlwaysAutoResize))
			{
				ImGui::InputTextWithHint(
					"File Name",
					"Enter file name",
					&newSceneName_);
				if (PokFileSystem::CheckFileExists(
					PokFileSystem::GetFullPath(
						newSceneName_,
						FileType::SCENE,
						FolderType::SAVE)
				)
					) {
					ImGui::Text(
						"File %s.pokscene already exists, the scene will be override.",
						newSceneName_.c_str());
				}
				if (ImGui::Button("Cancel")) { ImGui::CloseCurrentPopup(); }
				ImGui::SameLine();
				if (!newSceneName_.empty()) {
					if (ImGui::Button("Save")) {
						scene::Scene newScene(
							SceneManagerLocator::Get().GetHighestSceneIndex() + 1,
							newSceneName_,
							newSceneName_);

						PokFileSystem::WriteFile(
							newSceneName_,
							sceneManager_.GetActiveScene().ToJson(),
							FileType::SCENE,
							FolderType::SAVE_IN_ROM);

						sceneManager_.AddScene(newScene);
						sceneManager_.LoadScene(newScene.GetSceneIndex());

						ImGui::CloseCurrentPopup();
					}
				}
				ImGui::EndPopup();
			}
		} // End not display showroom
		
		//Guizmo list
		GizmoUtility::DisplayGizmoMenu("Gizmo");

        

		if (editorEcsManager_.HasComponent(selectedEntityIndex_, ecs::ComponentType::TRANSFORM)) 
		{
			math::Transform worldTransform = editorEcsManager_.GetComponentsManager<ecs::TransformsManager>().GetWorldTransform(selectedEntityIndex_);
			math::Transform modifiedTransform = ComponentUtility::DisplayTransformEditorGizmo(worldTransform);
            if(worldTransform != modifiedTransform) {
				editorEcsManager_.GetComponentsManager<ecs::TransformsManager>().SetComponentFromWorldTransform(selectedEntityIndex_, modifiedTransform);
            }
		}

		ComponentUtility::DisplayTransformEditor();

		//Game buttons
		if (game_.GetState() == AppState::STOP) {
			if (ImGui::Button("Play")) { StartGame(); }
		}
		else {
			if (ImGui::Button("Stop")) { StopGame(); }
			if (game_.IsPaused()) {
				if (ImGui::Button("Unpause")) {
					CameraLocator::Assign(game_.GetCameraPtr());
					game_.UnPause();
				}
			}
			else {
				if (ImGui::Button("Pause")) {
					CameraLocator::Assign(&camera_);
					game_.Pause();
				}
			}
		}

		ImGui::EndMenuBar();
	}

	GizmoUtility::DisplayAllChunksGizmo();

	//Update all tool
	for (auto& tool : tools_) { tool->UpdateActiveState(); }

	//Draw all tools
	for (auto& tool : tools_) { if (tool->IsActive()) { tool->OnDrawImgui(); } }

	if (showDemoWindow_) { ImGui::ShowDemoWindow(&showDemoWindow_); }

	GizmoUtility::DisplayGizmos();

    //Keyboard shortcuts
	if (inputManager_.IsKeyDown(input::KeyCode::ESCAPE) && game_.GetState() != AppState::STOP) {
		if (game_.IsPaused()) {
			game_.UnPause();
		}
		else {
			game_.Pause();
		}
	}
	if (inputManager_.IsKeyHeld(input::KeyCode::KEY_LEFT_CTRL) &&
		inputManager_.IsKeyDown(input::KeyCode::P)) {
		if (game_.GetState() == AppState::STOP) {
			StartGame();
		}
		else {
			StopGame();
		}
	}


	//Close drawing of imgui
	ImGui::End();
}

} //namespace editor
} //namespace poke
