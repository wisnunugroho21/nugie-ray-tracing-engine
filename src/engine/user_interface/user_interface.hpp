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
      /* data */
    public:
      EngineUserInterface(EngineDevice &engineDevice, GLFWwindow* window, std::shared_ptr<EngineHybridRenderer> renderer, 
        std::shared_ptr<EngineSwapChainSubRenderer> subRenderer);
      ~EngineUserInterface();

      void render(std::shared_ptr<EngineCommandBuffer> commandBuffer);
      
  };
}
