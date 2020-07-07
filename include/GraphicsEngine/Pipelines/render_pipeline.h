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
// Co-Author :
// Date : 29.01.20
//-----------------------------------------------------------------------------
#pragma once

#include <GraphicsEngine/Pipelines/pipeline.h>

namespace poke {
namespace graphics {

enum class SubrendererIndex : uint8_t{
    DEFERRED = 0,
    GIZMO,
    IMGUI,
    OPAQUE,
    PARTICLES,
    TRANSPARENT,
    FILTER_DEFAULT,
    FILTER_BLUR,
    FILTER_FXAA,
    FILTER_RIPPLE
};

template <typename T>
class HasGetSubrendererIndex
{
private:
	typedef char YesType[1];
	typedef char NoType[2];

	template <typename C> static YesType& test(decltype(&C::GetSubrendererIndex));
	template <typename C> static NoType& test(...);


public:
	enum { value = sizeof(test<T>(0)) == sizeof(YesType) };
};

/**
 * \brief Class used to insert pipeline into ECS system
 */
class RenderPipeline {
public:
    /**
     * \brief 
     * \param stage 
     */
    explicit RenderPipeline(
        const Pipeline::Stage stage)
        : stage_(stage),
          enabled_(true) {}

    virtual ~RenderPipeline() = default;

    virtual void OnRender(const CommandBuffer& commandBuffer) = 0;

    const Pipeline::Stage& GetStage() const { return stage_; }

    bool IsEnabled() const { return enabled_; }

    void SetEnabled(const bool& enable) { enabled_ = enable; }

private:
    Pipeline::Stage stage_;
    bool enabled_;
};
} //namespace graphics
} //namespace poke
