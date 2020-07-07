//-----------------------------------------------------------------------------
// Copyright (c) 2019-2020, POK Family. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  * Neither the name of POK Family nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
// OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Author : Nicolas Schneider
// Co-Author : Everybody
// Date : 09.12.19
//-----------------------------------------------------------------------------
#pragma once

#include <Ecs/ComponentManagers/models_manager.h>
#include <Ecs/core_ecs_manager.h>
#include <Ecs/system.h>
#include <GraphicsEngine/Models/model_command_buffer.h>
#include <GraphicsEngine/Gizmos/gizmo_command_buffer.h>

namespace poke {
struct EngineExport;

class TestModule final : public ecs::System {
public:
    explicit TestModule(Engine& engine);
    ~TestModule();
    void OnAppBuild() override;
    void OnLoadScene() override;
    void OnUpdate();
    void OnPhysicUpdate();
    void OnDraw();

    void OnEntityInit(ecs::EntityIndex entityIndex) override;
private:
    Engine& engine_;
    int testValue_ = 10;

    ecs::ModelsManager& modelsManager_;
    ecs::TransformsManager& transformsManager_;

    ecs::EntityIndex index_{};

    ecs::EntityIndex otherEntity0_;
    ecs::EntityIndex otherEntity1_;
    ecs::EntityIndex otherEntity2_;

    std::experimental::optional<graphics::ModelCommandBuffer&>
    modelCommandBuffer_;
    std::experimental::optional<graphics::GizmoCommandBuffer&>
    gizmoCommandBuffer_;
};
} //namespace poke