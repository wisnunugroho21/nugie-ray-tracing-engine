#include "user_interface.hpp"

namespace nugiEngine
{
  EngineUserInterface::EngineUserInterface(EngineDevice &engineDevice, GLFWwindow* window, 
    std::shared_ptr<EngineHybridRenderer> renderer, std::shared_ptr<EngineSwapChainSubRenderer> subRenderer,
    std::shared_ptr<EngineCommandBuffer> commandBuffer)
  {
    ImGui::CreateContext();
    this->io = ImGui::GetIO(); (void) this->io;

    ImGui_ImplGlfw_InitForVulkan(window, false);
    
    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = engineDevice.getInstance();
    init_info.PhysicalDevice = engineDevice.getPhysicalDevice();
    init_info.Device = engineDevice.getLogicalDevice();
    init_info.QueueFamily = engineDevice.getQueueFamilyIndices().graphicsFamily;
    init_info.Queue = engineDevice.getGraphicsQueue(0);
    init_info.DescriptorPool = renderer->getDescriptorPool()->getDescriptorPool();
    init_info.Subpass = 0;
    init_info.MinImageCount = 2;
    init_info.ImageCount = static_cast<int>(renderer->getSwapChain()->imageCount());
    init_info.MSAASamples = engineDevice.getMSAASamples();

    ImGui_ImplVulkan_Init(&init_info, subRenderer->getRenderPass()->getRenderPass());

    bool isCommandBufferCreatedHere = false;
    
    if (commandBuffer == nullptr) {
      commandBuffer = std::make_shared<EngineCommandBuffer>(engineDevice);
      commandBuffer->beginSingleTimeCommand();

      isCommandBufferCreatedHere = true;  
    }

    ImGui_ImplVulkan_CreateFontsTexture(commandBuffer->getCommandBuffer());

    if (isCommandBufferCreatedHere) {
      commandBuffer->endCommand();
      commandBuffer->submitCommand(engineDevice.getTransferQueue(0));
    }

    ImGui::GetIO().WantCaptureMouse;
  }

  EngineUserInterface::~EngineUserInterface() {
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
  }

  void EngineUserInterface::render(std::shared_ptr<EngineCommandBuffer> commandBuffer, std::shared_ptr<CameraPosition> cameraPosition) {
    if (!this->showUI) {
      return;
    }

    ImGui_ImplGlfw_NewFrame();
    ImGui_ImplVulkan_NewFrame();
    ImGui::NewFrame();

    const float distance = 10.0f;
    const ImVec2 pos = ImVec2(distance, distance);
    const ImVec2 posPivot = ImVec2(0.0f, 0.0f);
    ImGui::SetNextWindowPos(pos, ImGuiCond_Always, posPivot);

    if (ImGui::Begin("Hello, world!", &this->showUI, ImGuiWindowFlags_AlwaysAutoResize)) {
      ImGui::Text("Camera Position");
      ImGui::Separator();
      ImGui::SliderFloat("X", &cameraPosition->lookFrom.x, 0.0f, 550.0f, "%.2f");
      ImGui::SliderFloat("Y", &cameraPosition->lookFrom.y, 0.0f, 550.0f, "%.2f");
      ImGui::SliderFloat("Z", &cameraPosition->lookFrom.z, -800.0f, 0.0f, "%.2f");
    }

    ImGui::End();
    ImGui::Render();
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer->getCommandBuffer());
  }
} // namespace name

