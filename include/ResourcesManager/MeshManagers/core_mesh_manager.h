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

#include <GraphicsEngine/Models/mesh.h>
#include <ResourcesManager/MeshManagers/interface_mesh_manager.h>
#include <xxhash.h>
#include <ResourcesManager/MeshManagers/mesh_obj.h>
#include <ResourcesManager/MeshManagers/mesh_box.h>
#include <ResourcesManager/MeshManagers/mesh_sphere.h>
#include <ResourcesManager/MeshManagers/mesh_plane.h>
#include <ResourcesManager/MeshManagers/mesh_quad.h>
#include <ResourcesManager/MeshManagers/mesh_line_gizmo.h>
#include <ResourcesManager/MeshManagers/mesh_box_gizmo.h>
#include <ResourcesManager/MeshManagers/mesh_sphere_gizmo.h>
#include <json.hpp>

namespace poke {
class Engine;

enum class MeshPrimitive : uint8_t {
    VOID = 0,
    CUBE,
    SPHERE,
    PLANE,
    QUAD,
    LINE_GIZMO,
    CUBE_GIZMO,
    SPHERE_GIZMO,
    LENGTH
};

/**
 * \brief Store all created mesh
 */
class CoreMeshManager : public IMeshManager {
public:
    /**
     * \brief 
     */
    CoreMeshManager();

    void Init() override;

	void AddMesh(const std::string& name) override;

    DynamicMeshIndex CreateDynamicMesh() override;

    ResourceID GetDynamicMeshResourceID(DynamicMeshIndex dynamicMeshIndex) override;

    void UpdateDynamicMesh(DynamicMeshIndex dynamicMeshIndex, const std::vector<graphics::VertexMesh>& vertices, const std::vector<uint32_t>& indexes) override;

    void DestroyDynamicMesh(DynamicMeshIndex dynamicMeshIndex) override;

    graphics::Mesh& GetMesh(const std::string& name) override;

    graphics::Mesh& GetMesh(ResourceID resourceID) override;

    graphics::Mesh& GetSphere() override;

    graphics::Mesh& GetCube() override;

    graphics::Mesh& GetPlane() override;

    graphics::Mesh& GetQuad() override;

    graphics::Mesh& GetLineGizmo() override;

    graphics::Mesh& GetCubeGizmo() override;

    graphics::Mesh& GetSphereGizmo() override;

    ResourceID GetSphereID() override;

    ResourceID GetCubeID() override;

    ResourceID GetPlaneID() override;

    ResourceID GetQuadID() override;

    ResourceID GetLineID() override;

    ResourceID GetCubeGizmoID() override;

    ResourceID GetSphereGizmoID() override;

    void Clear() override;

    void Resize(size_t newSize) override;

    void SetFromJson(const json& meshesJson) override;

    json ToJson() override;
protected:
	std::vector<XXH64_hash_t> meshIDs_;
    std::vector<graphics::Mesh> meshes_;

	std::vector<graphics::Mesh> dynamicMeshes_;
	std::vector<XXH64_hash_t> dynamicMeshIDs_;

	DynamicMeshIndex nextFreeDynamicMesh_ = 0;

    MeshBox cubePrimitive_;
    MeshSphere spherePrimitive_;
    MeshPlane planePrimitive_;
    MeshQuad quadPrimitive_;
    MeshLineGizmo lineGizmoPrimitive_;
    MeshBoxGizmo cubeGizmoPrimitive_;
    MeshSphereGizmo sphereGizmoPrimitive_;

	inline static const size_t kMeshObjDefaultSize = 1000;
};
} //namespace poke
