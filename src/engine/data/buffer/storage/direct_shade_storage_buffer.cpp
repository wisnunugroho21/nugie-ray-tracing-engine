#include "direct_shade_storage_buffer.hpp"

#include <cstring>
#include <iostream>
#include <unordered_map>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

namespace NugieApp {
	DirectShadeStorageBuffer::DirectShadeStorageBuffer(NugieVulkan::Device* device, NugieVulkan::CommandBuffer *commandBuffer, uint32_t dataCount) : device{device} {
		auto datas = std::vector<DirectShadeRecord>();
		for (uint32_t i = 0; i < dataCount; i++) {
			DirectShadeRecord data{};
			datas.emplace_back(data);
		}

		this->createBuffers(commandBuffer, datas);
	}

	std::vector<VkDescriptorBufferInfo> DirectShadeStorageBuffer::getBuffersInfo() {
		std::vector<VkDescriptorBufferInfo> buffersInfo{};
		
		for (uint32_t i = 0; i < this->buffers.size(); i++) {
			buffersInfo.emplace_back(this->buffers.at(static_cast<size_t>(i))->descriptorInfo());
		}

		return buffersInfo;
	}

	void DirectShadeStorageBuffer::createBuffers(NugieVulkan::CommandBuffer *commandBuffer, std::vector<DirectShadeRecord> datas) {
		auto bufferSize = static_cast<VkDeviceSize>(sizeof(DirectShadeRecord));
		auto instanceCount = static_cast<uint32_t>(datas.size());

		this->buffers.clear();

		for (uint32_t i = 0; i < NugieVulkan::Device::MAX_FRAMES_IN_FLIGHT; i++) {
			NugieVulkan::Buffer stagingBuffer {
				this->device,
				bufferSize,
				instanceCount,
				VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
			};

			stagingBuffer.map();
			stagingBuffer.writeToBuffer(datas.data());

			auto buffer = std::make_shared<NugieVulkan::Buffer>(
				this->device,
				bufferSize,
				instanceCount,
				VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			);

			buffer->copyFromAnotherBuffer(&stagingBuffer, commandBuffer);
			this->buffers.emplace_back(buffer);
		}
	}

	void DirectShadeStorageBuffer::transferToRead(NugieVulkan::CommandBuffer* commandBuffer, uint32_t frameIndex) {
		this->buffers.at(static_cast<size_t>(frameIndex))->transitionBuffer(VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, 
			VK_ACCESS_SHADER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, commandBuffer);
	}

	void DirectShadeStorageBuffer::transferToWrite(NugieVulkan::CommandBuffer* commandBuffer, uint32_t frameIndex) {
		this->buffers.at(static_cast<size_t>(frameIndex))->transitionBuffer(VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, 
			VK_ACCESS_SHADER_READ_BIT, VK_ACCESS_SHADER_WRITE_BIT, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, commandBuffer);
	} 
} // namespace NugieApp

