#include <GraphicsEngine/RenderStage/attachment.h>

namespace poke {
namespace graphics {

Attachment::Attachment()
    : binding_(0),
      name_(),
      type_(Type::NONE),
      multisampled_(false),
      format_(VK_FORMAT_R8G8B8A8_UNORM) {}

Attachment::Attachment(
    const uint32_t binding,
    const char* name,
    const Type type,
    const bool multisampled,
    const VkFormat format
)
    : binding_(binding),
      name_(name),
      type_(type),
      multisampled_(multisampled),
      format_(format) {}
} //namespace graphics
} //namespace poke
