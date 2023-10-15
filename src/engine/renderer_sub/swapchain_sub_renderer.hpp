#pragma once

#include "../../vulkan/image/image.hpp"
#include "../../vulkan/renderpass/renderpass.hpp"

#include <vulkan/vulkan.h>
#include <vector>
#include <memory>

namespace NugieApp {
  class SwapChainSubRenderer {
    public:
      SwapChainSubRenderer(NugieVulkan::Device* device, std::vector<NugieVulkan::Image*> swapChainImages, VkFormat swapChainImageFormat, uint32_t imageCount, uint32_t width, uint32_t height);
      
      NugieVulkan::RenderPass* getRenderPass() const { return this->renderPass.get(); }

      void beginRenderPass(NugieVulkan::CommandBuffer* commandBuffer, int currentImageIndex);
			void endRenderPass(NugieVulkan::CommandBuffer* commandBuffer);
      
    private:
      uint32_t width, height;
      NugieVulkan::Device* device;

      std::vector<NugieVulkan::Image> colorImages;
      std::vector<NugieVulkan::Image> depthImages;
      std::vector<NugieVulkan::Image*> swapChainImages;
      
      std::unique_ptr<NugieVulkan::RenderPass> renderPass;

      VkFormat findDepthFormat();

      void createColorResources(VkFormat swapChainImageFormat, uint32_t imageCount);
      void createDepthResources(uint32_t imageCount);
      void createRenderPass(VkFormat swapChainImageFormat, uint32_t imageCount);
  };
  
} // namespace NugieApp
