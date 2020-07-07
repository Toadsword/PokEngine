#include <GraphicsEngine/Particles/emitter_container.h>

namespace poke {
namespace graphics {
EmitterContainer::EmitterContainer() : type(EmitterType::POINT) { }

EmitterContainer::EmitterContainer(Emitter& emitter, const EmitterType type)
    : type(type)
{
    switch (type) {
    case EmitterType::CIRCLE:
        circle = reinterpret_cast<EmitterCircle&>(emitter);
        break;
    case EmitterType::LINE:
        line = reinterpret_cast<EmitterLine&>(emitter);
        break;
    case EmitterType::POINT:
        point = reinterpret_cast<EmitterPoint&>(emitter);
        break;
    case EmitterType::SPHERE:
        sphere = reinterpret_cast<EmitterSphere&>(emitter);
        break;
    case EmitterType::CONE:
        cone = reinterpret_cast<EmitterCone&>(emitter);
        break;
    default: ;
    }
}

EmitterContainer::~EmitterContainer()
{
    switch (type) {
    case EmitterType::CIRCLE:
        break;
    case EmitterType::LINE:
        break;
    case EmitterType::POINT:
        break;
    case EmitterType::SPHERE:
        break;
    case EmitterType::CONE:
        break;
    default: ;
    }
}

bool EmitterContainer::operator==(const EmitterContainer& other) const
{
    if (type != other.type) { return false; }

    switch (type) {
    case EmitterType::CIRCLE:
        if (circle != other.circle) { return false; }
        break;
    case EmitterType::LINE:
        if (line != other.line) { return false; }
        break;
    case EmitterType::POINT:
        if (point != other.point) { return false; }
        break;
    case EmitterType::SPHERE:
        if (sphere != other.sphere) { return false; }
        break;
    case EmitterType::CONE:
        if (cone != other.cone) { return false; }
        break;
    default: ;
    }
    return true;
}

bool EmitterContainer::operator!=(const EmitterContainer& other) const { return !(*this == other); }

EmitterContainer::EmitterContainer(const EmitterContainer& other)
{
    type = other.type;

    switch (type) {
    case EmitterType::CIRCLE:
        circle = other.circle;
        break;
    case EmitterType::LINE:
        line = other.line;
        break;
    case EmitterType::POINT:
        point = other.point;
        break;
    case EmitterType::SPHERE:
        sphere = other.sphere;
        break;

    case EmitterType::CONE:
        cone = other.cone;
        break;
    default: ;
    }
}

EmitterContainer::EmitterContainer(EmitterContainer&& other) noexcept
{
    type = other.type;

    switch (type) {
    case EmitterType::CIRCLE:
        circle = other.circle;
        break;
    case EmitterType::LINE:
        line = other.line;
        break;
    case EmitterType::POINT:
        point = other.point;
        break;
    case EmitterType::SPHERE:
        sphere = other.sphere;
        break;
    case EmitterType::CONE:
        cone = other.cone;
        break;
    default: ;
    }
}

EmitterContainer& EmitterContainer::operator=(const EmitterContainer& other)
{
    type = other.type;

    switch (type) {
    case EmitterType::CIRCLE:
        circle = other.circle;
        break;
    case EmitterType::LINE:
        line = other.line;
        break;
    case EmitterType::POINT:
        point = other.point;
        break;
    case EmitterType::SPHERE:
        sphere = other.sphere;
        break;
    case EmitterType::CONE:
        cone = other.cone;
        break;
    default: ;
    }
    return *this;
}

EmitterContainer& EmitterContainer::operator=(EmitterContainer&& other) noexcept
{
    type = other.type;
    switch (type) {
    case EmitterType::CIRCLE:
        circle = other.circle;
        break;
    case EmitterType::LINE:
        line = other.line;
        break;
    case EmitterType::POINT:
        point = other.point;
        break;
    case EmitterType::SPHERE:
        sphere = other.sphere;
        break;
    case EmitterType::CONE:
        cone = other.cone;
        break;
    default: ;
    }
    return *this;
}

math::Vec3 EmitterContainer::GetSpawnPos() const
{
    math::Vec3 pos;

    switch (type) {
    case EmitterType::CIRCLE: { pos = circle.GeneratePosition(); }
    break;
    case EmitterType::LINE: { pos = line.GeneratePosition(); }
    break;
    case EmitterType::POINT: { pos = point.GeneratePosition(); }
    break;
    case EmitterType::SPHERE: { pos = sphere.GeneratePosition(); }
    break;
    case EmitterType::CONE: { pos = cone.GeneratePosition(); }
    break;
    default: ;
    }

    return pos;
}

math::Vec3 EmitterContainer::GetDirection(const math::Vec3& spawnPos) const
{
    math::Vec3 direction;

    switch (type) {
    case EmitterType::CIRCLE: { direction = circle.GenerateDirection(spawnPos); }
    break;
    case EmitterType::LINE: { direction = line.GenerateDirection(spawnPos); }
    break;
    case EmitterType::POINT: { direction = point.GenerateDirection(spawnPos); }
    break;
    case EmitterType::SPHERE: { direction = sphere.GenerateDirection(spawnPos); }
    break;
    case EmitterType::CONE: { direction = cone.GenerateDirection(spawnPos); }
    break;
    default: ;
    }

    return direction;
}

void EmitterContainer::SetFromJson(const json& emitterJson)
{
    type = emitterJson["type"];

    switch (type) {
    case EmitterType::CIRCLE:
        circle.SetFromJson(emitterJson["shape"]);
        break;
    case EmitterType::LINE:
        line.SetFromJson(emitterJson["shape"]);
        break;
    case EmitterType::POINT:
        point.SetFromJson(emitterJson["shape"]);
        break;
    case EmitterType::SPHERE:
        sphere.SetFromJson(emitterJson["shape"]);
        break;
    case EmitterType::CONE:
        cone.SetFromJson(emitterJson["shape"]);
        break;
    default: ;
    }
}

json EmitterContainer::ToJson() const
{
    json emitterJson;
    emitterJson["type"] = type;

    switch (type) {
    case EmitterType::CIRCLE:
        emitterJson["shape"] = circle.ToJson();
        break;
    case EmitterType::LINE:
        emitterJson["shape"] = line.ToJson();
        break;
    case EmitterType::POINT:
        emitterJson["shape"] = point.ToJson();
        break;
    case EmitterType::SPHERE:
        emitterJson["shape"] = sphere.ToJson();
        break;
    case EmitterType::CONE:
        emitterJson["shape"] = cone.ToJson();
        break;
    default: ;
    }

    return emitterJson;
}
} //namespace graphics
} //namespace poke
