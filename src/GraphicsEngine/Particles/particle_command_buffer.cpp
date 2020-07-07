#include <GraphicsEngine/Particles/particle_command_buffer.h>

#include <CoreEngine/engine.h>
#include <GraphicsEngine/Particles/particle_instance.h>

namespace poke {
namespace graphics {

ParticleCommandBuffer::ParticleCommandBuffer(Engine& engine)
{
    engine.AddObserver(
        observer::MainLoopSubject::ENGINE_DESTROY,
        [this]() { OnEngineDestroy(); });
}

void ParticleCommandBuffer::DrawParticles(
	const int index,
    const std::vector<ParticleDrawInfo>& particles)
{
	instanceBuffers_[index]->Update(particles);
}

ParticleInstanceIndex ParticleCommandBuffer::AddParticleInstance(const Material& material)
{
	for (size_t i = 0; i < instanceBuffers_.size(); i++) {
		if (&instanceBuffers_[i]->GetMaterial() == &material) {
			 return i;
		}
	}

    instanceBuffers_.push_back(nullptr);
	instanceBuffers_.back() = std::make_unique<ParticleInstance>(material);
	return instanceBuffers_.size() - 1;
}

void ParticleCommandBuffer::RemoveParticleInstance(const ParticleInstanceIndex index)
{
	instanceBuffers_[index] = nullptr;
}

std::vector<std::unique_ptr<ParticleInstance>>& ParticleCommandBuffer::
GetParticleDrawCommands() { return instanceBuffers_; }

void ParticleCommandBuffer::OnEngineDestroy()
{
    for (auto& particleInstance : instanceBuffers_) {
		particleInstance.reset();
    }
}
} //namespace graphics
} //namespace poke
