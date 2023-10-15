#include "indirect_data_storage_buffer.hpp"

#include <cstring>
#include <iostream>
#include <unordered_map>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

namespace NugieApp {
	IndirectDataStorageBuffer::IndirectDataStorageBuffer(NugieVulkan::Device* device, NugieVulkan::CommandBuffer *commandBuffer, uint32_t dataCount) : device{device} {
		auto datas = std::vector<RenderResult>();
		for (uint32_t i = 0; i < dataCount; i++) {
			RenderResult data{};

			data.totalRadiance = glm::vec3{0.0f};
			data.totalIndirect = glm::vec3{1.0f};
			data.pdf = 1.0f;
			
			datas.emplace_back(data);
		}

		this->createBuffers(commandBuffer, datas);
	}

	std::vector<VkDescriptorBufferInfo> IndirectDataStorageBuffer::getBuffersInfo() {
		std::vector<VkDescriptorBufferInfo> buffersInfo{};
		
		for (int i = 0; i < this->buffers.size(); i++) {
			buffersInfo.emplace_back(this->buffers.at(static_cast<size_t>(i))->descriptorInfo());
		}

		return buffersInfo;
	}

	void IndirectDataStorageBuffer::createBuffers(NugieVulkan::CommandBuffer *commandBuffer, std::vector<RenderResult> datas) {
		auto bufferSize = static_cast<VkDeviceSize>(sizeof(RenderResult));
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
} // namespace NugieApp

