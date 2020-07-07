#include <GraphicsEngine/Guis/ui_transform.h>

namespace poke {
namespace graphics {
UiTransform::UiTransform(math::Vec2 size, const math::Vec2 anchor, const math::Vec2 position)
    : size_(size),
      anchor0_(anchor),
      anchor1_(anchor),
      position_(position) { }

UiTransform::UiTransform(
    const math::Vec2 size,
    const math::Vec2 anchor0,
    const math::Vec2 anchor1,
    const math::Vec2 position)
    : size_(size),
      anchor0_(anchor0),
      anchor1_(anchor1),
      position_(position) { }

UiTransform::UiTransform(
    const UiMargins margins,
    const math::Vec2 anchor0,
    const math::Vec2 anchor1)
    : anchor0_(anchor0),
      anchor1_(anchor1),
      margins_(margins) { }

bool UiTransform::operator==(const UiTransform& other) const
{
    return
        size_ == other.size_ && 
		anchor0_ == other.anchor0_ &&
		anchor1_ == other.anchor1_ &&
        position_ == other.position_ &&
        margins_ == other.margins_ && 
		depth_ == other.depth_;
}

bool UiTransform::operator!=(const UiTransform& other) const { return !operator==(other); }

void UiTransform::SetAnchor(const math::Vec2 anchor)
{
    anchor0_ = anchor;
    anchor1_ = anchor;
}
} //namespace graphics
} //namespace poke
