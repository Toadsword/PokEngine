#include <GraphicsEngine/Lights/light_command_buffer.h>

#include <CoreEngine/engine.h>

namespace poke {
namespace graphics {

LightCommandBuffer::LightCommandBuffer(Engine& engine)
{
    engine.AddObserver(
        observer::MainLoopSubject::ENGINE_INIT,
        [this]() {
            GraphicsEngineLocator::Get().GetEngine().AddObserver(
                observer::MainLoopSubject::UPDATE,
                [this]() { OnUpdate(); });
            GraphicsEngineLocator::Get().GetEngine().AddObserver(
                observer::MainLoopSubject::END_FRAME,
                [this]() { OnEndOfFrame(); });
        }
    );

    spotLightDrawCmds_.reserve(maxSpotLight);
    pointLightDrawCmds_.reserve(maxPointLight);

    directionalLightDrawCmd_ = {Color::white, {0, -1, 0}};
}

LightCommandBuffer::~LightCommandBuffer()
{
    //std::cout << "~LightCommandBuffer\n";
}

void LightCommandBuffer::Draw(const DirectionalLightDrawCommand cmd)
{
    directionalLightDrawCmd_ = cmd;
}

void LightCommandBuffer::Draw(SpotLightDrawCommand cmd)
{
    if (spotLightDrawCmds_.size() < maxSpotLight) { spotLightDrawCmds_.emplace_back(cmd); }
}

void LightCommandBuffer::Draw(PointLightDrawCommand cmd)
{
    if (pointLightDrawCmds_.size() < maxPointLight) { pointLightDrawCmds_.emplace_back(cmd); }
}

DirectionalLightDrawCommand LightCommandBuffer::GetDirectionalCmds() const
{
    return directionalLightDrawCmd_;
}

const std::vector<SpotLightDrawCommand>& LightCommandBuffer::
GetSpotLightDrawCmds() const { return spotLightDrawCmds_; }

const std::vector<PointLightDrawCommand>& LightCommandBuffer::
GetPointLightDrawCmds() const { return pointLightDrawCmds_; }

void LightCommandBuffer::Clear()
{
    spotLightDrawCmds_.clear();
    pointLightDrawCmds_.clear();
}

void LightCommandBuffer::OnUpdate() {}

void LightCommandBuffer::OnEndOfFrame() { Clear(); }
} //namespace graphics
} //namespace poke
