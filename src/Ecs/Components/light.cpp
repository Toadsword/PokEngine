#include <Ecs/Components/light.h>

namespace poke {
namespace ecs {

Light::Light(): pointLight() { }

bool Light::operator==(const Light& rhs) const
{
    if (lightType != rhs.lightType) { return false; }

    switch (lightType) {
    case graphics::LightType::DIRECTIONAL:
        return directionalLight == rhs.directionalLight;
    case graphics::LightType::SPOT:
        return spotLight == rhs.spotLight;
    case graphics::LightType::POINT:
        return pointLight == rhs.pointLight;
    default: ;
    }

    return true;
}

bool Light::operator!=(const Light& rhs) const { return !(*this == rhs); }

void Light::SetFromJson(const json& lightJson)
{
    lightType = lightJson["type"];

    switch (lightType) {
    case graphics::LightType::DIRECTIONAL:
        directionalLight.color.SetFromJson(lightJson["color"]);
        directionalLight.direction.SetFromJson(lightJson["direction"]);
        directionalLight.intensity = lightJson["intensity"];
        break;
    case graphics::LightType::SPOT:
        spotLight.color.SetFromJson(lightJson["color"]);
        spotLight.direction.SetFromJson(lightJson["direction"]);
        spotLight.angleInDeg = lightJson["spotAngle"];
        spotLight.intensity = lightJson["intensity"];
        spotLight.range = lightJson["range"];
        break;
    case graphics::LightType::POINT:
        pointLight.color.SetFromJson(lightJson["color"]);
        pointLight.areaSize.SetFromJson(lightJson["radius"]);
        pointLight.intensity = lightJson["intensity"];
        break;
    case graphics::LightType::LENGTH:
        break;
    default: ;
    }
}

json Light::ToJson() const
{
    json lightJson;
    lightJson["type"] = lightType;

    switch (lightType) {
    case graphics::LightType::DIRECTIONAL:
        lightJson["color"] = directionalLight.color.ToJson();
        lightJson["direction"] = directionalLight.direction.ToJson();
        lightJson["intensity"] = directionalLight.intensity;
        break;
    case graphics::LightType::SPOT:
        lightJson["color"] = spotLight.color.ToJson();
        lightJson["direction"] = spotLight.direction.ToJson();
        lightJson["spotAngle"] = spotLight.angleInDeg;
        lightJson["intensity"] = spotLight.intensity;
        lightJson["range"] = spotLight.range;
        break;
    case graphics::LightType::POINT:
        lightJson["color"] = pointLight.color.ToJson();
        lightJson["radius"] = pointLight.areaSize.ToJson();
        lightJson["intensity"] = pointLight.intensity;
        break;
    case graphics::LightType::LENGTH:
        break;
    default: ;
    }

    return lightJson;
}
} //namespace ecs
} //namespace poke