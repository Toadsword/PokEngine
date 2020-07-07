#include <GraphicsEngine/RenderStage/subpass_type.h>

namespace poke {
namespace graphics {

SubpassType::SubpassType(
    const uint32_t binding,
    const std::vector<uint32_t>& attachmentBindings)
    : binding_(binding),
      attachmentBindings_(attachmentBindings) {}

const std::vector<uint32_t>& SubpassType::GetAttachmentBindings() const
{
    return attachmentBindings_;
}

void SubpassType::SetIsMultisampled(const bool isMultisampled)
{
    isMultisample_ = isMultisampled;
}
} //namespace graphics
} //namespace poke
