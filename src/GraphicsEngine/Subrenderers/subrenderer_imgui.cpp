#include <GraphicsEngine/Subrenderers/subrenderer_imgui.h>

#include <imgui.h>
#include <examples/imgui_impl_sdl.h>
#include <examples/imgui_impl_vulkan.h>

#include <CoreEngine/engine.h>
#include <GraphicsEngine/vulkan_error_handler.h>
#include <Utility/file_system.h>

#include <CoreEngine/ServiceLocator/service_locator_definition.h>

#define IMGUI_VULKAN_DEBUG_REPORT

namespace poke {
namespace graphics {
SubrendererImgui::SubrendererImgui(
    Pipeline::Stage pipelineStage)
    : RenderPipeline(pipelineStage)
{
	GraphicsEngineLocator::Get().AddWindowResizeObserver([this]() {OnWindowResize(); });
	GraphicsEngineLocator::Get().GetEngine().AddObserver(observer::MainLoopSubject::DRAW, [this]() {OnDrawImGui(); });
	GraphicsEngineLocator::Get().GetEngine().AddObserver(observer::MainLoopSubject::END_FRAME, [this]() {OnEndOfFrame(); });

    const auto& surface = GraphicsEngineLocator::Get().GetSurface();
    const auto& logicalDevice = GraphicsEngineLocator::Get().GetLogicalDevice();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    // Color scheme
    // Dimensions
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui::StyleColorsDark();

    VkDescriptorPoolSize poolSizes[] =
    {
        {VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
        {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
        {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
        {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
        {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
        {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
        {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
        {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
        {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000}
    };

    VkDescriptorPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    poolInfo.maxSets = 1000 * IM_ARRAYSIZE(poolSizes);
    poolInfo.poolSizeCount = static_cast<uint32_t>(IM_ARRAYSIZE(poolSizes));
    poolInfo.pPoolSizes = poolSizes;
    CheckVk(
        vkCreateDescriptorPool(
            logicalDevice,
            &poolInfo,
            nullptr,
            &descriptorPool_));

    ImGui_ImplSDL2_InitForVulkan(GraphicsEngineLocator::Get().GetWindow().GetWindowPtr());


    const auto surfaceCapabilities = surface.GetCapabilities();

    ImGui_ImplVulkan_InitInfo initInfo = {};
    initInfo.Instance = poke::GraphicsEngineLocator::Get().GetInstance();
    initInfo.PhysicalDevice = poke::GraphicsEngineLocator::Get().GetPhysicalDevice();
    initInfo.Device = logicalDevice;
    initInfo.QueueFamily = logicalDevice.GetGraphicsFamily();
    initInfo.Queue = logicalDevice.GetGraphicsQueue();
    initInfo.PipelineCache = VK_NULL_HANDLE;
    initInfo.DescriptorPool = descriptorPool_;
    initInfo.Allocator = nullptr;
    initInfo.MinImageCount = surfaceCapabilities.minImageCount;
    initInfo.ImageCount = poke::GraphicsEngineLocator::Get()
                          .GetSwapchain().GetImageCount();
    initInfo.CheckVkResultFn = CheckVk;
    initInfo.Subpass = pipelineStage.second;
    ImGui_ImplVulkan_Init(
        &initInfo,
        GraphicsEngineLocator::Get()
        .GetRenderStage(0).GetRenderPass()->GetRenderPass());

    // Create font texture
    unsigned char* fontData;
    int texWidth, texHeight;

    //MODIFICATION FROM SIMON-------------------------------------------------------------------------
    ImFontConfig fontConfig;
    fontConfig.OversampleH = 2;
    fontConfig.OversampleV = 2;
    fontConfig.PixelSnapH = true;

    io.Fonts->AddFontFromFileTTF(
        PokFileSystem::GetFullPath(
            "unifont",
            FileType::FONTS,
            FolderType::ROM).
        c_str(),
        16,
        &fontConfig);
    //------------------------------------------------------------------------------------------------

    io.Fonts->GetTexDataAsRGBA32(&fontData, &texWidth, &texHeight);

    // Upload Fonts
    {
        // Use any command queue
        auto commandBuffer = CommandBuffer();

        ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);

        commandBuffer.SubmitIdle();

        CheckVk(vkDeviceWaitIdle(logicalDevice));
        ImGui_ImplVulkan_DestroyFontUploadObjects();
    }

    //Start drawing
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplSDL2_NewFrame(
        GraphicsEngineLocator::Get().GetWindow().GetWindowPtr());
    ImGui::NewFrame();
}

SubrendererImgui::~SubrendererImgui()
{
	//std::cout << "~SubrendererImgui();\n";
    const auto& logicalDevice = GraphicsEngineLocator::Get().GetLogicalDevice();
    vkDestroyDescriptorPool(logicalDevice, descriptorPool_, nullptr);

    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}

void SubrendererImgui::OnRender(const CommandBuffer& commandBuffer)
{
    //End drawing
    ImGui::Render();
    ImGui_ImplVulkan_RenderDrawData(
        ImGui::GetDrawData(),
        commandBuffer.GetCommandBuffer());
	hasBeenDrawn_ = true;
    //Start drawing
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplSDL2_NewFrame(
        GraphicsEngineLocator::Get().GetWindow().GetWindowPtr());
    ImGui::NewFrame();
}

void SubrendererImgui::OnWindowResize()
{
	//End drawing
	ImGui::Render();
	ImGui::GetDrawData()->Clear();

	//Start drawing
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplSDL2_NewFrame(GraphicsEngineLocator::Get().GetWindow().GetWindowPtr());
	ImGui::NewFrame();
}

void SubrendererImgui::OnEndOfFrame()
{
    if(!hasBeenDrawn_) {
		//End drawing
		ImGui::Render();
		ImGui::GetDrawData()->Clear();

		//Start drawing
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplSDL2_NewFrame(GraphicsEngineLocator::Get().GetWindow().GetWindowPtr());
		ImGui::NewFrame();
    }
	hasBeenDrawn_ = false;
}

void SubrendererImgui::OnDrawImGui() { /*hasBeenDrawn_ = false;*/ }
} //namespace graphics
} //namespace poke
