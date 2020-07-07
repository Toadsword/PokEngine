#include <GraphicsEngine/RenderStage/viewport.h>

namespace poke {
namespace graphics {

Viewport::Viewport()
    : scale_(1.0f, 1.0f),
      offset_(0, 0),
      hasSize_(false) {}

Viewport::Viewport(const math::Vec2Int size)
    : scale_(1.0f, 1.0f),
      offset_(0, 0),
      size_(size),
      hasSize_(true) { }

Viewport::Viewport(const Viewport& v)
    : scale_(1.0f, 1.0f),
      offset_(0, 0),
      size_(v.size_),
      hasSize_(v.hasSize_) { }
} //namespace graphics
} //namespace poke
