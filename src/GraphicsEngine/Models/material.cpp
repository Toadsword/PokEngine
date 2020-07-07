#include <GraphicsEngine/Models/material.h>

namespace poke {
namespace graphics {

const Material::PushDataContainer& Material::ExportDescriptorData() const
{
    return descriptorData_;
}

const PipelineMaterial& Material::GetPipelineMaterial() const
{
    if(!pipelineMaterial_) {
		std::cout << "ici\n";
    }

    return *pipelineMaterial_;
}

bool Material::BindPipeline(const CommandBuffer& commandBuffer) const
{
    return pipelineMaterial_->BindPipeline(commandBuffer);
}
} //namespace graphics
} //namespace poke