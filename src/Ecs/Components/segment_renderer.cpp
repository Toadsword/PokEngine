#include <Ecs/Components/segment_renderer.h>

#include <CoreEngine/ServiceLocator/service_locator_definition.h>

namespace poke {
namespace ecs {

SegmentRenderer::SegmentRenderer()
{
    materialID = math::HashString("diffuse_test");
}

bool SegmentRenderer::operator==(const SegmentRenderer& other) const
{
    return lifetime == other.lifetime
           && width == other.width
           && materialID == other.materialID;
}

bool SegmentRenderer::operator!=(const SegmentRenderer& other) const { return !(*this == other); }

void SegmentRenderer::SetFromJson(const json& segmentRendererJson)
{
    lifetime = segmentRendererJson["lifetime"];
    width = segmentRendererJson["width"];
    materialID = segmentRendererJson["materialID"];
}

json SegmentRenderer::ToJson()
{
    json segmentRendererJson;

    segmentRendererJson["lifetime"] = lifetime;
    segmentRendererJson["width"] = width;
    segmentRendererJson["materialID"] = materialID;

    return segmentRendererJson;
}
} //namespace ecs
} //namespace poke