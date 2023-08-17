#include "indirect_shade_storage_buffer.hpp"

#include <cstring>
#include <iostream>
#include <unordered_map>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

namespace nugiEngine {
	EngineIndirectShadeStorageBuffer::EngineIndirectShadeStorageBuffer(EngineDevice &device, uint32_t dataCount) : engineDevice{device} {
		auto datas = std::make_shared<std::vector<IndirectShadeRecord>>();
		for (uint32_t i = 0; i < dataCount; i++) {
			IndirectShadeRecord data{};
			datas->emplace_back(data);
		}

		this->createBuffers(datas);
	}

	std::vector<VkDescriptorBufferInfo> EngineIndirectShadeStorageBuffer::getBuffersInfo() {
		std::vector<VkDescriptorBufferInfo> buffersInfo{};
		
		for (int i = 0; i < this->buffers->size(); i++) {
			buffersInfo.emplace_back(this->buffers->at(i).descriptorInfo());
		}

		return buffersInfo;
	}

	void EngineIndirectShadeStorageBuffer::createBuffers(std::shared_ptr<std::vector<IndirectShadeRecord>> datas) {
		auto bufferSize = static_cast<VkDeviceSize>(sizeof(IndirectShadeRecord));
		auto instanceCount = static_cast<uint32_t>(datas->size());
		auto totalSize = static_cast<VkDeviceSize>(bufferSize * instanceCount);

		this->buffers = std::make_shared<std::vector<EngineBuffer>>();

		for (uint32_t i = 0; i < EngineDevice::MAX_FRAMES_IN_FLIGHT; i++) {
			EngineBuffer stagingBuffer {
				this->engineDevice,
				bufferSize,
				instanceCount,
				VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
			};

			stagingBuffer.map();
			stagingBuffer.writeToBuffer(datas->data());

			EngineBuffer buffer {
				this->engineDevice,
				bufferSize,
				instanceCount,
				VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			};

			buffer.copyBuffer(stagingBuffer.getBuffer(), totalSize);
			this->buffers->emplace_back(buffer);
		}
	}

	void EngineIndirectShadeStorageBuffer::transferToRead(std::shared_ptr<EngineCommandBuffer> commandBuffer, uint32_t frameIndex) {
		this->buffers->at(static_cast<size_t>(frameIndex)).transitionBuffer(VK_SHADER_STAGE_COMPUTE_BIT, VK_SHADER_STAGE_COMPUTE_BIT, 
			VK_ACCESS_SHADER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, commandBuffer);
	}

	void EngineIndirectShadeStorageBuffer::transferToWrite(std::shared_ptr<EngineCommandBuffer> commandBuffer, uint32_t frameIndex) {
		this->buffers->at(static_cast<size_t>(frameIndex)).transitionBuffer(VK_SHADER_STAGE_COMPUTE_BIT, VK_SHADER_STAGE_COMPUTE_BIT, 
			VK_ACCESS_SHADER_READ_BIT, VK_ACCESS_SHADER_WRITE_BIT, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, commandBuffer);
	} 
} // namespace nugiEngine

