#pragma once

#include "../../vulkan/window/window.hpp"
#include "../../vulkan/device/device.hpp"
#include "../../vulkan/swap_chain/swap_chain.hpp"
#include "../../vulkan/buffer/buffer.hpp"
#include "../../vulkan/descriptor/descriptor_pool.hpp"
#include "../../vulkan/command/command_pool.hpp"
#include "../../vulkan/command/command_buffer.hpp"
#include "../ray_ubo.hpp"

#include <memory>
#include <vector>
#include <cassert>

namespace NugieApp {
	class HybridRenderer
	{
		public:
			HybridRenderer(NugieVulkan::Window* window, NugieVulkan::Device* device);
			~HybridRenderer();

			NugieVulkan::SwapChain* getSwapChain() const { return this->swapChain.get(); }
			NugieVulkan::DescriptorPool* getDescriptorPool() const { return this->descriptorPool.get(); }
			bool isFrameInProgress() const { return this->isFrameStarted; }

			VkCommandBuffer getCommandBuffer() const { 
				assert(this->isFrameStarted && "cannot get command buffer when frame is not in progress");
				return this->commandBuffers[this->currentFrameIndex]->getCommandBuffer();
			}

			uint32_t getFrameIndex() const {
				assert(this->isFrameStarted && "cannot get frame index when frame is not in progress");
				return this->currentFrameIndex;
			}

			uint32_t getImageIndex() const {
				assert(this->isFrameStarted && "cannot get frame index when frame is not in progress");
				return this->currentImageIndex;
			}

			NugieVulkan::CommandBuffer* beginCommand();
			NugieVulkan::CommandBuffer* beginTransferCommand();

			void submitCommands(std::vector<NugieVulkan::CommandBuffer*> commandBuffer);
			void submitCommand(NugieVulkan::CommandBuffer* commandBuffer);

			void submitTransferCommand(NugieVulkan::CommandBuffer* commandBuffer);

			bool acquireFrame();
			bool presentFrame();

		private:
			void recreateSwapChain();
			void createSyncObjects(uint32_t imageCount);
			void createDescriptorPool();
			void createCommandPool();
			void createCommandBuffers();

			NugieVulkan::Window* window;
			NugieVulkan::Device* device;

			std::vector<std::unique_ptr<NugieVulkan::CommandBuffer>> commandBuffers;
			std::vector<std::unique_ptr<NugieVulkan::CommandBuffer>> transferCommandBuffers;

			std::unique_ptr<NugieVulkan::CommandPool> commandPool;
			std::unique_ptr<NugieVulkan::DescriptorPool> descriptorPool;
			std::unique_ptr<NugieVulkan::SwapChain> swapChain;

			std::vector<VkSemaphore> imageAvailableSemaphores;
			std::vector<VkSemaphore> renderFinishedSemaphores;
			std::vector<VkFence> inFlightFences;
			std::vector<VkFence> transferFences;

			uint32_t currentImageIndex = 0, currentFrameIndex = 0;
			bool isFrameStarted = false;
	};
}