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
#include <Math/matrix.h>

namespace poke {

using FrustumPlanes = std::array<math::Vec4, 6>;

class ICamera {
public:
    virtual ~ICamera() = default;

    virtual void Update() = 0;

    virtual math::Matrix4 GetProjectionMatrix() const = 0;
    virtual void SetProjectionMatrix(math::Matrix4 projectionMatrix) = 0;

    virtual math::Matrix4 GetViewMatrix() const = 0;
    virtual void SetViewMatrix(math::Matrix4 viewMatrix) = 0;

    virtual math::Vec3 GetPosition() const = 0;
    virtual void SetPosition(math::Vec3 position) = 0;

    virtual math::Vec3 GetRotation() const = 0;
    virtual void SetRotation(math::Vec3 rotation) = 0;

    virtual math::Vec3 GetVelocity() const = 0;
    virtual void SetVelocity(math::Vec3 velocity) = 0;

    virtual float GetFieldOfView() const = 0;
    virtual void SetFieldOfView(float fieldOfView) = 0;

    virtual float GetNearPlan() const = 0;
    virtual void SetNearPlan(float nearPlan) = 0;

    virtual float GetFarPlan() const = 0;
    virtual void SetFarPlan(float farPlan) = 0;

    virtual FrustumPlanes GetFrustumPlanes() = 0;

    virtual math::Vec3 GetFront() const = 0;
    virtual math::Vec3 GetUp() const = 0;
    virtual math::Vec3 GetRight() const = 0;
};
} //namespace poke
