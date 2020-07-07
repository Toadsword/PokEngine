#include <ResourcesManager/MeshManagers/mesh_obj.h>

#include <unordered_map>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

namespace poke {
std::unique_ptr<MeshObj> MeshObj::Create(const std::string& filename)
{
	auto mesh = std::make_unique<MeshObj>();
	mesh->Load(filename);
    return mesh;
}

MeshObj::MeshObj()
{
    
}

void MeshObj::Load(const std::string& filename)
{
    if (filename.empty()) { return; }

    auto filePath = PokFileSystem::GetFullPath(
		filename,
        FileType::MESH);

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!LoadObj(
        &attrib,
        &shapes,
        &materials,
        &warn,
        &err,
        filePath.c_str())) { cassert(false, warn + err); }

    std::vector<graphics::VertexMesh> vertices;
    std::vector<uint32_t> indices;
    std::unordered_map<graphics::VertexMesh, size_t> uniqueVertices;

    size_t count = 0;
    for (const auto& shape : shapes) { count += shape.mesh.indices.size(); }
    indices.reserve(count);

    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            const int vectorSize = 3;
            //Position
            auto position = math::Vec3(
                attrib.vertices[vectorSize * index.vertex_index + 0],
                attrib.vertices[vectorSize * index.vertex_index + 1],
                attrib.vertices[vectorSize * index.vertex_index + 2]);

            //UV
            math::Vec2 uv;
            if (index.texcoord_index > 0) {
                uv = math::Vec2(
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    1.0f - attrib.texcoords[2 * index.texcoord_index + 1]);
            }

            //Normal
            math::Vec3 normal;
            if (index.normal_index > 0) {
                normal = math::Vec3(
                    attrib.normals[vectorSize * index.normal_index + 0],
                    attrib.normals[vectorSize * index.normal_index + 1],
                    attrib.normals[vectorSize * index.normal_index + 2]);
            }
            auto vertex = graphics::VertexMesh(position, uv, normal);
            if (uniqueVertices.count(vertex) == 0) {
                uniqueVertices[vertex] = vertices.size();
                vertices.emplace_back(vertex);
            }
            indices.emplace_back(static_cast<uint32_t>(uniqueVertices[vertex]));
        }
    }
    Initialize(vertices, indices);
}
} //namespace poke
