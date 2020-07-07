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
// Date : 30.01.20
//-----------------------------------------------------------------------------
#pragma once
#include <string>
#include <ResourcesManager/resource_type.h>

namespace poke {
namespace graphics {
class Mesh;
} //namespace graphics

using DynamicMeshIndex = uint16_t;

/**
 * \brief Store all created mesh
 */
class IMeshManager {
public:
    virtual ~IMeshManager() = default;

    virtual void Init() = 0;

    /**
	 * \brief Add a mesh in the mesh manager by loading a json.
	 * \param name 
	 */
    virtual void AddMesh(const std::string& name) = 0;

	virtual DynamicMeshIndex CreateDynamicMesh() = 0;

	virtual ResourceID GetDynamicMeshResourceID(DynamicMeshIndex dynamicMeshIndex) = 0;

    /**
     * \brief 
     * \param dynamicMeshIndex 
     * \param vertices 
     * \param indexes 
     */
    virtual void UpdateDynamicMesh(DynamicMeshIndex dynamicMeshIndex, const std::vector<graphics::VertexMesh>& vertices, const std::vector<uint32_t>& indexes) = 0;

	virtual void DestroyDynamicMesh(DynamicMeshIndex dynamicMeshIndex) = 0;

    //TODO(@nico) return const &
    /**
     * \brief Get a mesh by its name.
     * \details This function is slower than when using a hash.
     * \param name of the mesh with the extension. 
     * \return 
     */
    virtual graphics::Mesh& GetMesh(const std::string& name) = 0;
    /**
     * \brief Get a mesh by its hash.
     * \param resourceID 
     * \return 
     */
    virtual graphics::Mesh& GetMesh(ResourceID resourceID) = 0;

    /**
	 * \brief Get the sphere's primitive.
	 * \return 
	 */
    virtual graphics::Mesh& GetSphere() = 0;

    /**
	 * \brief Get the id of the sphere primitive.
	 * \return 
	 */
    virtual ResourceID GetSphereID() = 0;

    /**
     * \brief Get the cube's primitive.
     * \return
     */
    virtual graphics::Mesh& GetCube() = 0;

    /**
     * \brief Get the id of the cube primitive.
     * \return
     */
    virtual ResourceID GetCubeID() = 0;

    /**
     * \brief Get the plane's primitive.
     * \return
     */
    virtual graphics::Mesh& GetPlane() = 0;

    /**
     * \brief Get the id of the plane primitive.
     * \return
     */
    virtual ResourceID GetPlaneID() = 0;

    /**
     * \brief Get the quad's primitive.
     * \return
     */
    virtual graphics::Mesh& GetQuad() = 0;
    /**
     * \brief Get the id of the quad primitive.
     * \return
     */
    virtual ResourceID GetQuadID() = 0;

    /**
     * \brief Get the line-gizmo's primitive.
     * \return
     */
    virtual graphics::Mesh& GetLineGizmo() = 0;
    /**
     * \brief Get the id of the line-gizmo primitive.
     * \return
     */
    virtual ResourceID GetLineID() = 0;

    /**
     * \brief Get the cube-gizmo's primitive.
     * \return
     */
    virtual graphics::Mesh& GetCubeGizmo() = 0;
    /**
     * \brief Get the id of the cube-gizmo primitive.
     * \return
     */
    virtual ResourceID GetCubeGizmoID() = 0;

    /**
     * \brief Get the sphere-gizmo's primitive.
     * \return
     */
    virtual graphics::Mesh& GetSphereGizmo() = 0;
    /**
     * \brief Get the id of the sphere-gizmo primitive.
     * \return
     */
    virtual ResourceID GetSphereGizmoID() = 0;

    /**
     * \brief Delete all texture registered.
     */
    virtual void Clear() = 0;

    /**
     * \brief Resize the manager to the given size. If the size is smaller than the actual size and some texture are still in, they will be erased.
     * \param newSize must be greater than 0
     */
    virtual void Resize(size_t newSize) = 0;

	virtual void SetFromJson(const json& meshesJson) = 0;

	virtual json ToJson() = 0;
};
} //namespace poke
