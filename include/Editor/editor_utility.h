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
// Author : Duncan Bourquard
// Co-Author :
// Date : 10.12.18
//----------------------------------------------------------------------------------
#pragma once

#include <cinttypes>
#include <string>

class ImVec4;

namespace poke {
class Color;

namespace editor {

enum ToolFlagEnum : uint16_t { 
	TOOL_FLAG_NONE                  = 0, 
    TOOL_FLAG_COMPONENT             = 1 << 0,
    TOOL_FLAG_CONVERTIBLE_SCENE     = 1 << 1,
    TOOL_FLAG_EXAMPLE               = 1 << 2,
    TOOL_FLAG_LOCALIZATION          = 1 << 3,
    TOOL_FLAG_LOG                   = 1 << 4,
    TOOL_FLAG_SHADER_CONVERTER      = 1 << 5,
    TOOL_FLAG_STATS                 = 1 << 6,
	TOOL_FLAG_INPUT_BINDING			= 1 << 7,
	TOOL_FLAG_PREFAB_CREATOR		= 1 << 8,
	TOOL_FLAG_PARTICLE				= 1 << 9,
	TOOL_FLAG_RESOURCES_VIEWER		= 1 << 10,
	TOOL_FLAG_GRAPHIC_CONTROLLER 	= 1 << 11,
	TOOL_FLAG_CHUNK_EDITOR			= 1 << 12,
	TOOL_FLAG_GIZMO_DISPLAY         = 1 << 13,


    TOOL_FLAG_ALL                   = (1 << 14) - 1
};

using ToolFlags = int;

/**
 * \brief Transform an Imgui Color to a Pok one
 * \param imGuiColor
 * \return
 */
Color ImGuiToPokColor(ImVec4 imGuiColor);
/**
 * \brief Transform a Pok Color to an Imgui one
 * \param pokColor
 * \return
 */
ImVec4 PokToImGuiColor(Color pokColor);
} //namespace editor
} //namespace poke
