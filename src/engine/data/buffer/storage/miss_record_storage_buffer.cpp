#include "miss_record_storage_buffer.hpp"

#include <cstring>
#include <iostream>
#include <unordered_map>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

namespace nugiEngine {
	EngineMissRecordStorageBuffer::EngineMissRecordStorageBuffer(EngineDevice &device, uint32_t dataCount) : engineDevice{device} {
		auto datas = std::make_shared<std::vector<MissRecord>>();
		for (uint32_t i = 0; i < dataCount; i++) {
			MissRecord data{};
			datas->emplace_back(data);
		}

		this->createBuffers(datas);
	}

	std::vector<VkDescriptorBufferInfo> EngineMissRecordStorageBuffer::getBuffersInfo() {
		std::vector<VkDescriptorBufferInfo> buffersInfo{};
		
		for (int i = 0; i < this->buffers->size(); i++) {
			buffersInfo.emplace_back((*this->buffers)[i].descriptorInfo());
		}

		return buffersInfo;
	}

	void EngineMissRecordStorageBuffer::createBuffers(std::shared_ptr<std::vector<MissRecord>> datas) {
		auto bufferSize = static_cast<VkDeviceSize>(sizeof(MissRecord));
		auto instanceCount = static_cast<uint32_t>(datas->size());
		auto totalSize = static_cast<VkDeviceSize>(bufferSize * instanceCount);

		this->buffers->clear();

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

	void EngineMissRecordStorageBuffer::transferToRead(std::shared_ptr<EngineCommandBuffer> commandBuffer, uint32_t frameIndex) {
		this->buffers->at(static_cast<size_t>(frameIndex)).transitionBuffer(VK_SHADER_STAGE_COMPUTE_BIT, VK_SHADER_STAGE_COMPUTE_BIT, 
			VK_ACCESS_SHADER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, commandBuffer);
	}

	void EngineMissRecordStorageBuffer::transferToWrite(std::shared_ptr<EngineCommandBuffer> commandBuffer, uint32_t frameIndex) {
		this->buffers->at(static_cast<size_t>(frameIndex)).transitionBuffer(VK_SHADER_STAGE_COMPUTE_BIT, VK_SHADER_STAGE_COMPUTE_BIT, 
			VK_ACCESS_SHADER_READ_BIT, VK_ACCESS_SHADER_WRITE_BIT, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, commandBuffer);
	} 
} // namespace nugiEngine

