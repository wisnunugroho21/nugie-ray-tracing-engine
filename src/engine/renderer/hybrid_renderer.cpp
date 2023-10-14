#include "hybrid_renderer.hpp"

#include <stdexcept>
#include <array>
#include <string>

namespace NugieApp {
	HybridRenderer::HybridRenderer(NugieVulkan::Window* window, NugieVulkan::Device* device) {
		this->recreateSwapChain();
		this->createSyncObjects(static_cast<uint32_t>(this->swapChain->imageCount()));

		this->createCommandPool();
		this->createDescriptorPool();

		for (uint32_t i = 0; i < NugieVulkan::Device::MAX_FRAMES_IN_FLIGHT; i++) {
			this->commandBuffers.emplace_back(std::make_unique<NugieVulkan::CommandBuffer>(this->device));
		}
	}

	HybridRenderer::~HybridRenderer() {
		this->descriptorPool->reset();
		
    for (size_t i = 0; i < NugieVulkan::Device::MAX_FRAMES_IN_FLIGHT; i++) {
			vkDestroySemaphore(this->device->getLogicalDevice(), this->renderFinishedSemaphores[i], nullptr);
			vkDestroySemaphore(this->device->getLogicalDevice(), this->imageAvailableSemaphores[i], nullptr);
			vkDestroyFence(this->device->getLogicalDevice(), this->inFlightFences[i], nullptr);
		}
	}

	void HybridRenderer::recreateSwapChain() {
		auto extent = this->window->getExtent();
		while(extent.width == 0 || extent.height == 0) {
			extent = this->window->getExtent();
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(this->device->getLogicalDevice());

		if (this->swapChain == nullptr) {
			this->swapChain = std::make_unique<NugieVulkan::SwapChain>(this->device, extent);
		} else {
			std::shared_ptr<NugieVulkan::SwapChain> oldSwapChain = std::move(this->swapChain);
			this->swapChain = std::make_unique<NugieVulkan::SwapChain>(this->device, extent, oldSwapChain);

			if (!oldSwapChain->compareSwapFormat(*this->swapChain.get())) {
				throw std::runtime_error("Swap chain image has changed");
			}
		}
	}

	void HybridRenderer::createDescriptorPool() {
		this->descriptorPool = 
			NugieVulkan::DescriptorPool::Builder(this->device)
				.setMaxSets(100)
				.addPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 100)
				.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 100)
				.addPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 100)
				.build();
	}

	void HybridRenderer::createCommandPool() {
		auto queueFamily = this->device->getPhysicalQueueFamilies();
		this->commandPool = std::make_unique<NugieVulkan::CommandPool>(
			this->device, 
			queueFamily.graphicsFamily, 
			VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT
		);
	}

	void HybridRenderer::createSyncObjects(uint32_t imageCount) {
		imageAvailableSemaphores.resize(NugieVulkan::Device::MAX_FRAMES_IN_FLIGHT);
		renderFinishedSemaphores.resize(NugieVulkan::Device::MAX_FRAMES_IN_FLIGHT);
		inFlightFences.resize(NugieVulkan::Device::MAX_FRAMES_IN_FLIGHT);

		VkSemaphoreCreateInfo semaphoreInfo = {};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo = {};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (size_t i = 0; i < NugieVulkan::Device::MAX_FRAMES_IN_FLIGHT; i++) {
		  if (vkCreateSemaphore(this->device->getLogicalDevice(), &semaphoreInfo, nullptr, &this->imageAvailableSemaphores[i]) != VK_SUCCESS ||
				vkCreateSemaphore(this->device->getLogicalDevice(), &semaphoreInfo, nullptr, &this->renderFinishedSemaphores[i]) != VK_SUCCESS ||
				vkCreateFence(this->device->getLogicalDevice(), &fenceInfo, nullptr, &this->inFlightFences[i]) != VK_SUCCESS) 
		  {
				throw std::runtime_error("failed to create synchronization objects for a frame!");
		  }
		}
	}

	bool HybridRenderer::acquireFrame() {
		assert(!this->isFrameStarted && "can't acquire frame while frame still in progress");

		std::vector<VkFence> acquireFrameFences = { this->inFlightFences[this->currentFrameIndex] };
		auto result = this->swapChain->acquireNextImage(&this->currentImageIndex, acquireFrameFences, this->imageAvailableSemaphores[this->currentFrameIndex]);
		
		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			this->recreateSwapChain();
			return false;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("failed to acquire swap chain image");
		}

		this->isFrameStarted = true;
		return true;
	}

	NugieVulkan::CommandBuffer* HybridRenderer::beginCommand() {
		assert(this->isFrameStarted && "can't start command while frame still in progress");

		this->commandBuffers[this->currentFrameIndex]->beginReccuringCommand();
		return this->commandBuffers[this->currentFrameIndex].get();
	}

	void HybridRenderer::endCommand(NugieVulkan::CommandBuffer* commandBuffer) {
		assert(this->isFrameStarted && "can't start command while frame still in progress");
		commandBuffer->endCommand();
	}

	void HybridRenderer::submitCommands(std::vector<NugieVulkan::CommandBuffer*> commandBuffers) {
		assert(this->isFrameStarted && "can't submit command if frame is not in progress");
		vkResetFences(this->device->getLogicalDevice(), 1, &this->inFlightFences[this->currentFrameIndex]);

		std::vector<VkSemaphore> waitSemaphores = {this->imageAvailableSemaphores[this->currentFrameIndex]};
		std::vector<VkSemaphore> signalSemaphores = {this->renderFinishedSemaphores[this->currentFrameIndex]};
		std::vector<VkPipelineStageFlags> waitStages = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

		NugieVulkan::CommandBuffer::submitCommands(commandBuffers, this->device->getGraphicsQueue(this->currentFrameIndex), waitSemaphores, waitStages, signalSemaphores, this->inFlightFences[this->currentFrameIndex]);
	}

	void HybridRenderer::submitCommand(NugieVulkan::CommandBuffer* commandBuffer) {
		assert(this->isFrameStarted && "can't submit command if frame is not in progress");
		vkResetFences(this->device->getLogicalDevice(), 1, &this->inFlightFences[this->currentFrameIndex]);

		std::vector<VkSemaphore> waitSemaphores = {this->imageAvailableSemaphores[this->currentFrameIndex]};
		std::vector<VkSemaphore> signalSemaphores = {this->renderFinishedSemaphores[this->currentFrameIndex]};
		std::vector<VkPipelineStageFlags> waitStages = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

		commandBuffer->submitCommand(this->device->getGraphicsQueue(this->currentFrameIndex), waitSemaphores, waitStages, signalSemaphores, this->inFlightFences[this->currentFrameIndex]);
	}

	bool HybridRenderer::presentFrame() {
		assert(this->isFrameStarted && "can't present frame if frame is not in progress");

		std::vector<VkSemaphore> waitSemaphores = {this->renderFinishedSemaphores[this->currentFrameIndex]};
		auto result = this->swapChain->presentRenders(this->device->getPresentQueue(this->currentFrameIndex), &this->currentImageIndex, waitSemaphores);

		this->currentFrameIndex = (this->currentFrameIndex + 1) % NugieVulkan::Device::MAX_FRAMES_IN_FLIGHT;

		this->isFrameStarted = false;

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || this->window->wasResized()) {
			this->window->resetResizedFlag();
			this->recreateSwapChain();
			this->descriptorPool->reset();

			return false;
		} else if (result != VK_SUCCESS) {
			throw std::runtime_error("failed to present swap chain image");
		}

		return true;
	}
}