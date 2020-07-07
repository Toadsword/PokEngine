#include <GraphicsEngine/Renderers/renderer_editor.h>

#include <CoreEngine/engine.h>
#include <CoreEngine/ServiceLocator/service_locator_definition.h>
#include <GraphicsEngine/Subrenderers/subrenderer_opaque.h>
#include <GraphicsEngine/Subrenderers/subrenderer_deferred.h>
#include <GraphicsEngine/Subrenderers/subrenderer_transparent.h>
#include <GraphicsEngine/Posts/filter_default.h>
#include <GraphicsEngine/Subrenderers/subrenderer_gizmo.h>
#include <GraphicsEngine/Subrenderers/subrenderer_imgui.h>
#include <GraphicsEngine/Subrenderers/subrenderer_particles.h>
#include <GraphicsEngine/Pipelines/pipeline_blur.h>
#include <GraphicsEngine/Posts/filter_fxaa.h>
#include <GraphicsEngine/Posts/filter_ripple.h>

namespace poke {
namespace graphics {
RendererEditor::RendererEditor(Engine& engine)
    : Renderer()
{
    engine.AddObserver(
        observer::MainLoopSubject::ENGINE_INIT,
        [this]() { this->OnEngineInit(); });
}

void RendererEditor::OnEngineInit()
{
    std::vector<Attachment> renderpassAttachment1 = {
        Attachment(0, "depth",     Attachment::Type::DEPTH,     false),
        Attachment(1, "swapchain", Attachment::Type::SWAPCHAIN, false, VK_FORMAT_R8G8B8A8_UNORM),
        Attachment(2, "position",  Attachment::Type::IMAGE,     false, VK_FORMAT_R16G16B16A16_SFLOAT),
        Attachment(3, "diffuse",   Attachment::Type::IMAGE,     false, VK_FORMAT_R8G8B8A8_UNORM),
        Attachment(4, "normal",    Attachment::Type::IMAGE,     false, VK_FORMAT_R16G16B16A16_SFLOAT),
        Attachment(5, "material",  Attachment::Type::IMAGE,     false, VK_FORMAT_R8G8B8A8_UNORM),
        Attachment(6, "resolved",  Attachment::Type::IMAGE,     false, VK_FORMAT_R16G16B16A16_SFLOAT),
        Attachment(7, "bright",    Attachment::Type::IMAGE,     false, VK_FORMAT_R16G16B16A16_SFLOAT)
    };

    std::vector<SubpassType> renderpassSubpasses1 = {
        SubpassType(0, {0, 2, 3, 4, 5}), //Geometry pass
        SubpassType(1, {0, 6, 7}), //Deferred pass
        SubpassType(2, {0, 1}) //Post process pass
    };
	
	AddRenderStage(std::make_unique<RenderStage>(renderpassAttachment1, renderpassSubpasses1));
}

void RendererEditor::Start()
{
	started_ = true;

    AddSubrenderer<SubrendererOpaque>(Pipeline::Stage(0, 0));
    AddSubrenderer<SubrendererDeferred>(Pipeline::Stage(0, 1));
    AddSubrenderer<SubrendererParticles>(Pipeline::Stage(0, 1));
    AddSubrenderer<SubrendererTransparent>(Pipeline::Stage(0, 1));

    AddSubrenderer<PipelineBlur>(Pipeline::Stage(0, 2), 1.8f);
    /*AddSubrenderer<FilterRipple>(Pipeline::Stage(0, 2));
    AddSubrenderer<FilterFxaa>(Pipeline::Stage(0, 2));*/
    AddSubrenderer<FilterDefault>(Pipeline::Stage(0, 2), true);
    AddSubrenderer<SubrendererGizmo>(Pipeline::Stage(0, 2));
    AddSubrenderer<SubrendererImgui>(Pipeline::Stage(0, 2));
}
} //namespace graphics
} //namespace poke
