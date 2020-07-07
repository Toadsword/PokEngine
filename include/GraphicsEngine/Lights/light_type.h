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
// Date : 26.02.20
//-----------------------------------------------------------------------------
#pragma once

namespace poke {
namespace graphics {
enum class LightType : uint8_t {
    DIRECTIONAL = 0,
    SPOT,
    POINT,
    LENGTH
};

static const std::string LightTypeToString(const LightType lightType)
{
    switch (lightType) {
    case LightType::DIRECTIONAL:
		return "Directional";
    case LightType::SPOT:
		return "Spot";
        break;
    case LightType::POINT:
		return "Point";
        break;
    case LightType::LENGTH:
		return "";
    default:
        return "";;
    }
}

static LightType StringToLightType(const std::string& name)
{
    if(name == "Directional") {
		return LightType::DIRECTIONAL;
    }

	if (name == "Point") {
		return LightType::POINT;
	}

    if(name == "Spot") {
		return LightType::SPOT;
    }

	cassert(false, "The name of the light is not a LightType");
}
} //namespace graphics
} //namespace poke
