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
#include <ResourcesManager/MeshManagers/interface_mesh_manager.h>
#include <CoreEngine/cassert.h>
#include <json.hpp>

namespace poke {
/**
 * \brief Null mesh manager
 */
class NullMeshManager final : public IMeshManager {
public:
    virtual ~NullMeshManager() = default;

    virtual void Init() override {}

	void AddMesh(const std::string& name) override
    {
		name;
    }

    graphics::Mesh& GetMesh(const std::string& name) override
    {
		name;
        cassert(false, "Impossible to acces NullMeshManager.");
		abort();
    }

	graphics::Mesh& GetMesh(ResourceID resourceID) override
    {
		resourceID;
		cassert(false, "Impossible to acces NullMeshManager.");
		abort();
    }


    graphics::Mesh& GetSphere() override
    {
        cassert(false, "Impossible to acces NullMeshManager.");
		abort();
    }

    graphics::Mesh& GetCube() override
    {
        cassert(false, "Impossible to acces NullMeshManager.");
		abort();
    }

    graphics::Mesh& GetPlane() override
    {
        cassert(false, "Impossible to acces NullMeshManager.");
		abort();
    }

    graphics::Mesh& GetQuad() override
    {
        cassert(false, "Impossible to acces NullMeshManager.");
		abort();
    }

    graphics::Mesh& GetLineGizmo() override
    {
        cassert(false, "Impossible to acces NullMeshManager.");
		abort();
    }

    graphics::Mesh& GetCubeGizmo() override
    {
        cassert(false, "Impossible to acces NullMeshManager.");
		abort();
    }

    graphics::Mesh& GetSphereGizmo() override
    {
        cassert(false, "Impossible to acces NullMeshManager.");
		abort();
    }

    ResourceID GetSphereID() override { return 0; }
    ResourceID GetCubeID() override { return 0; }
    ResourceID GetPlaneID() override { return 0; }
    ResourceID GetQuadID() override { return 0; }
    ResourceID GetLineID() override { return 0; }
    ResourceID GetCubeGizmoID() override { return 0; }
    ResourceID GetSphereGizmoID() override { return 0; }

    void Clear() override {}

    void Resize(size_t newSize) override
    {
		newSize;
    }

	void SetFromJson(const json& meshesJson) override
    {
		meshesJson;
    }

	json ToJson() override { return json(); }

	DynamicMeshIndex CreateDynamicMesh() override { return 0; }
	ResourceID GetDynamicMeshResourceID(DynamicMeshIndex dynamicMeshIndex) override { return 0; }
    void UpdateDynamicMesh(
        DynamicMeshIndex dynamicMeshIndex,
        const std::vector<graphics::VertexMesh>& vertices,
        const std::vector<uint32_t>& indexes) override{ }
    void DestroyDynamicMesh(DynamicMeshIndex dynamicMeshIndex) override{}
};
} //namespace poke
