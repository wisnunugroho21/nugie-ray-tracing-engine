#pragma once

#include "../../vulkan/device/device.hpp"
#include "../../vulkan/buffer/buffer.hpp"
#include "../../vulkan/command/command_buffer.hpp"
#include "../renderer/hybrid_renderer.hpp"
#include "../renderer_sub/swapchain_sub_renderer.hpp"

#include "../../../libraries/imgui/imgui.h"
#include "../../../libraries/imgui/imgui_impl_glfw.h"
#include "../../../libraries/imgui/imgui_impl_vulkan.h"

namespace nugiEngine {
  class EngineUserInterface
  {
    private:
      ImGuiIO io;
      bool showUI = true;

    public:
      EngineUserInterface(EngineDevice &engineDevice, GLFWwindow* window, std::shared_ptr<EngineHybridRenderer> renderer, 
        std::shared_ptr<EngineSwapChainSubRenderer> subRenderer, std::shared_ptr<EngineCommandBuffer> commandBuffer = nullptr);
      ~EngineUserInterface();

      bool wantToCaptureMouse() const { return ImGui::GetIO().WantCaptureMouse; }
      void render(std::shared_ptr<EngineCommandBuffer> commandBuffer, std::shared_ptr<CameraPosition> cameraPosition);
      
  };
}
