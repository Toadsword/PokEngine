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
// Date : 02.03.2020
//-----------------------------------------------------------------------------
#pragma once
#include <Utility/color.h>

namespace poke {
class Engine;
}

namespace poke {
namespace graphics {
struct PointLightDrawCommand {
	Color color = Color::white;
	math::Vec3 position = {0.0f,0.0f,0.0f};
	float radius = 0.0f;
};

struct SpotLightDrawCommand
{
	Color color = Color::white;
	math::Vec3 position = {0.0f,0.0f,0.0f};
	float range = 0.0f;
	math::Vec3 target = {0.0f,0.0f,0.0f};
	float angle = 0;
};

struct DirectionalLightDrawCommand
{
	Color color = Color::white;
	math::Vec3 direction = {0.0f, -1.0f, 0.0f};
};

class LightCommandBuffer {
public:
	explicit LightCommandBuffer(Engine& engine);
	~LightCommandBuffer();

	void Draw(DirectionalLightDrawCommand cmd);
	void Draw(SpotLightDrawCommand cmd);
	void Draw(PointLightDrawCommand cmd);

    DirectionalLightDrawCommand GetDirectionalCmds() const;
    const std::vector<SpotLightDrawCommand>& GetSpotLightDrawCmds() const;
    const std::vector<PointLightDrawCommand>& GetPointLightDrawCmds() const;

	void Clear();

	inline const static int maxDirectionalLight = 1;
	inline const static int maxSpotLight = 32;
	inline const static int maxPointLight = 32;

private:
	void OnUpdate();

	void OnEndOfFrame();

	DirectionalLightDrawCommand directionalLightDrawCmd_;
	std::vector<SpotLightDrawCommand> spotLightDrawCmds_;
	std::vector<PointLightDrawCommand> pointLightDrawCmds_;
};
} //namespace graphics
} //namespace poke