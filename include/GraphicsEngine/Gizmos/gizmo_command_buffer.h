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
// Date : 28.01.20
//-----------------------------------------------------------------------------
#pragma once

#include <GraphicsEngine/Gizmos/gizmo_instances.h>

namespace poke {
namespace math {
class Transform;
} //namespace math

class Engine;

namespace graphics {
enum class GizmoType : uint8_t {
    CUBE = 0,
    SPHERE,
    LINE,
    LENGTH
};

/**
 * \brief Store all command for drawing Gizmo
 */
class GizmoCommandBuffer {
public:
    /**
     * \brief
     * \param engine 
     */
    explicit GizmoCommandBuffer(Engine& engine);

    ~GizmoCommandBuffer();

    /**
     * \brief Draw a wired Cube
     * \param position 
     * \param extends
     * \param rotation 
     */
    void DrawWireCube(
        math::Vec3 position,
        math::Vec3 extends,
        math::Vec3 rotation = {});


    /**
     * \brief Draw a wired Cube
     * \param transform 
     */
    void DrawWireCube(math::Transform transform);

    /**
     * \brief Draw a wired sphere
     * \param position 
     * \param radius 
     */
    void DrawWireSphere(math::Vec3 position, float radius);

    /**
	 * \brief Draw a line from the start position to the end position
	 * \param start 
	 * \param end 
	 */
	void DrawLine(math::Vec3 start, math::Vec3 end);

    void DrawWireMesh(const Mesh& mesh);

	/**
	 * \brief Get the color for all following commands
	 */
	Color GetColor() const { return color_; }

    /**
     * \brief Set the color for all following commands
     * \param color 
     */
    void SetColor(Color color);

    /**
     * \brief Return all gizmo instance (Use in Render Thread)
     * \return 
     */
    std::vector<GizmoInstances>& GetGizmoInstances();

    void Clear();

private:
    struct AllocatorHead {
        int start;
        int head;
        int end;
    };

    void OnEngineInit();

    void OnEndOfFrame();

    void OnUnloadScene();

    void AddCmdToDraw(math::Matrix4 worldMatrix, Color c, GizmoType type);

    static const int kSizePerType = 2000;

    std::array<GizmoDrawCommand, kSizePerType * static_cast<int>(GizmoType::LENGTH)>
    gizmosDrawCommands_;
    std::array<AllocatorHead, static_cast<int>(GizmoType::LENGTH)> arraySize_{};

    std::vector<GizmoInstances> current_;
    std::vector<GizmoInstances> next_;

    Color color_ = Color::red;
};
} //namespace graphics
} //namespace poke
