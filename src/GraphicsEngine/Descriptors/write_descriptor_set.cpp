#include <GraphicsEngine/Descriptors/write_descriptor_set.h>

namespace poke {
namespace graphics {
WriteDescriptorSet::WriteDescriptorSet(
    VkWriteDescriptorSet writeDescriptorSet,
    VkDescriptorImageInfo imageInfo)
    : writeDescriptorSet_(writeDescriptorSet)
    , imageInfo_(imageInfo) {
    if (imageInfo_) { writeDescriptorSet_.pImageInfo = &(*imageInfo_); }
}

WriteDescriptorSet::WriteDescriptorSet(
    VkWriteDescriptorSet writeDescriptorSet,
    VkDescriptorBufferInfo bufferInfo)
    : writeDescriptorSet_(writeDescriptorSet)
    , bufferInfo_(bufferInfo) {
    writeDescriptorSet_.pBufferInfo = &*bufferInfo_;
}

VkWriteDescriptorSet WriteDescriptorSet::GetWriteDescriptorSet() {
    if (imageInfo_) { writeDescriptorSet_.pImageInfo = &*imageInfo_; } else if
    (bufferInfo_) { writeDescriptorSet_.pBufferInfo = &*bufferInfo_; }
    return writeDescriptorSet_;
}
} //namespace graphics
} //namespace poke
