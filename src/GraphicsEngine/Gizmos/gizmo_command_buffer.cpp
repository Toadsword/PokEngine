#include <GraphicsEngine/Gizmos/gizmo_command_buffer.h>

#include <vector>

#include <CoreEngine/engine.h>
#include <CoreEngine/ServiceLocator/service_locator_definition.h>
#include <Math/quaternion.h>

namespace poke {
namespace graphics {
GizmoCommandBuffer::GizmoCommandBuffer(Engine& engine)
{
    //Observe 
    engine.AddObserver(observer::MainLoopSubject::ENGINE_INIT, [this]() { OnEngineInit(); });
}

GizmoCommandBuffer::~GizmoCommandBuffer()
{
    //std::cout << "~GizmoCommandBuffer\n";
}

void GizmoCommandBuffer::DrawWireCube(
    const math::Vec3 position,
    const math::Vec3 extends,
    const math::Vec3 rotation)
{
    AddCmdToDraw(math::Transform(position, rotation, extends).GetWorldMatrix(), color_, GizmoType::CUBE);
}

void GizmoCommandBuffer::DrawWireCube(const math::Transform transform)
{
    AddCmdToDraw(transform.GetWorldMatrix(), color_ , GizmoType::CUBE);
}

void GizmoCommandBuffer::DrawWireSphere(
    const math::Vec3 position,
    const float radius)
{
    AddCmdToDraw(
        math::Transform(position, {0, 0, 0}, {radius, radius, radius}).GetWorldMatrix(),
        color_,
        GizmoType::SPHERE);
}

void GizmoCommandBuffer::DrawLine(const math::Vec3 start, const math::Vec3 end)
{
	math::Transform transform;
	transform.SetLocalPosition(end);
	const float distance = math::Vec3::GetDistance(start, end);
	transform.SetLocalScale(distance);

	auto p1 = math::Vec3{ 0, 1, 0 };
	p1 = p1.Normalize();
	auto p2 = end - start;
	p2 = p2.Normalize();

	const math::Quaternion quat = math::Quaternion::FromToRotation(p1, p2);
    
	const math::Vec3 rotation = quat.GetEulerAngles();

	transform.SetLocalRotation(rotation);

	const auto mat = transform.GetWorldMatrix();
	AddCmdToDraw(mat, color_, GizmoType::LINE);
}

void GizmoCommandBuffer::DrawWireMesh(const Mesh& mesh)
{

	auto indexes = mesh.GetIndices(0);
	std::vector<VertexMesh> vertices = mesh.GetVertices<VertexMesh>(0);
		
    for (int i = 0; i < indexes.size() - 3; i += 3) {
        DrawLine(vertices[indexes[i]].position, vertices[indexes[i + 1]].position);
        DrawLine(vertices[indexes[i + 1]].position, vertices[indexes[i + 2]].position);
        DrawLine(vertices[indexes[i + 2]].position, vertices[indexes[i]].position);
    }
}

void GizmoCommandBuffer::SetColor(const Color color) { color_ = color; }

void GizmoCommandBuffer::OnEndOfFrame()
{
	for (int i = 0; i < static_cast<int>(GizmoType::LENGTH); i++) {
		std::vector<GizmoDrawCommand> subVector(
			&gizmosDrawCommands_[arraySize_[i].start],
			&gizmosDrawCommands_[arraySize_[i].head]);
		current_[i].Update(subVector);
		arraySize_[i].head = arraySize_[i].start;
	}

    std::swap(current_, next_);
}

void GizmoCommandBuffer::OnUnloadScene()
{
	for (int i = 0; i < static_cast<int>(GizmoType::LENGTH); i++) {
		arraySize_[i].head = arraySize_[i].start;
	}
}

std::vector<GizmoInstances>& GizmoCommandBuffer::GetGizmoInstances()
{
    return next_;
}

void GizmoCommandBuffer::Clear()
{
    current_.clear();
    next_.clear();
}

void GizmoCommandBuffer::OnEngineInit()
{
    GraphicsEngineLocator::Get().GetEngine().GetModuleManager().sceneManager.
                                AddOnUnloadObserver(
                                    [this]() { OnUnloadScene(); });

	GraphicsEngineLocator::Get().GetEngine().AddObserver(
		observer::MainLoopSubject::END_FRAME,
		[this]() { OnEndOfFrame(); });

    //Add instance for gizmo model's specifics

    current_.reserve(static_cast<int>(GizmoType::LENGTH));
    current_.emplace_back(MeshManagerLocator::Get().GetCubeGizmo(), 1);
    current_.emplace_back(MeshManagerLocator::Get().GetSphereGizmo(), 1);
    current_.emplace_back(MeshManagerLocator::Get().GetLineGizmo(), 1);

    next_.reserve(static_cast<int>(GizmoType::LENGTH));
    next_.emplace_back(MeshManagerLocator::Get().GetCubeGizmo(), 1);
    next_.emplace_back(MeshManagerLocator::Get().GetSphereGizmo(), 1);
    next_.emplace_back(MeshManagerLocator::Get().GetLineGizmo(), 1);

    for (int i = 0; i < static_cast<int>(GizmoType::LENGTH); i++) {
        arraySize_[i] = {
            i * kSizePerType,
            i * kSizePerType,
            i * kSizePerType + kSizePerType
        };
    }
}

void GizmoCommandBuffer::AddCmdToDraw(
	const math::Matrix4 worldMatrix,
    const Color c,
    const GizmoType type)
{
    gizmosDrawCommands_[arraySize_[static_cast<int>(type)].head] = {worldMatrix, c};

    arraySize_[static_cast<int>(type)].head++;

    if (arraySize_[static_cast<int>(type)].head == arraySize_[static_cast<int>(type)].end) {
        arraySize_[static_cast<int>(type)].head = arraySize_[static_cast<int>(type)].end - 1;
    }
}
} //namespace graphics
} //namespace poke
