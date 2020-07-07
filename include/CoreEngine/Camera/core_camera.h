//----------------------------------------------------------------------------------
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
// Co-Author :
// Date : 18.04.20
//----------------------------------------------------------------------------------
#pragma once
#include <CoreEngine/Camera/interface_camera.h>

namespace poke {
class Engine;

class CoreCamera : public ICamera {
public:

    explicit CoreCamera(Engine& engine);
    virtual ~CoreCamera() = default;

    void Update() override;

    //---------------------------- GETTER / SETTER ----------------------------

    math::Matrix4 GetProjectionMatrix() const override;
    void SetProjectionMatrix(math::Matrix4 projectionMatrix) override;

    math::Matrix4 GetViewMatrix() const override;
    void SetViewMatrix(math::Matrix4 viewMatrix) override;

    math::Vec3 GetPosition() const override;
    void SetPosition(math::Vec3 position) override;

    math::Vec3 GetRotation() const override;
    void SetRotation(math::Vec3 rotation) override;

    math::Vec3 GetVelocity() const override;
    void SetVelocity(math::Vec3 velocity) override;

    float GetFieldOfView() const override;
    void SetFieldOfView(float fieldOfView) override;

    float GetNearPlan() const override;
    void SetNearPlan(float nearPlan) override;

    float GetFarPlan() const override;
    void SetFarPlan(float farPlan) override;

    math::Vec3 GetFront() const override { return front_; }
    math::Vec3 GetUp() const override { return up_; }
    math::Vec3 GetRight() const override { return right_; }


    //-------------------------------------------------------------------------

    //--------------------------------- FRUSTUM -------------------------------
    FrustumPlanes GetFrustumPlanes() override;
    //-------------------------------------------------------------------------

private:
    math::Matrix4 projectionMatrix_{1};
    math::Matrix4 viewMatrix_{1};

    math::Vec3 position_ = math::Vec3(0.0f, 0.0f, 0.0f);
    float fieldOfView_ = 60.0f;

    math::Vec3 rotation_ = math::Vec3(0.0f, 0.0f, 0.0f);
    float nearPlane_ = 0.1f;

    math::Vec3 velocity_ = math::Vec3(0.0f, 0.0f, 0.0f);
    float farPlane_ = 1000;

    math::Vec3 front_;
    math::Vec3 up_;
    math::Vec3 right_;
};
} //namespace poke
