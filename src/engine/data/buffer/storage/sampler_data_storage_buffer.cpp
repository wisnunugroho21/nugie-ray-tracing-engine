#include "sampler_data_storage_buffer.hpp"

#include <cstring>
#include <iostream>
#include <unordered_map>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

namespace nugiEngine {
	EngineSamplerDataStorageBuffer::EngineSamplerDataStorageBuffer(EngineDevice &device, std::shared_ptr<std::vector<SamplerData>> datas) : engineDevice{device} {
		this->createBuffers(datas);
	}

	std::vector<VkDescriptorBufferInfo> EngineSamplerDataStorageBuffer::getBuffersInfo() {
		std::vector<VkDescriptorBufferInfo> buffersInfo{};
		
		for (int i = 0; i < this->buffers.size(); i++) {
			buffersInfo.emplace_back(this->buffers.at(static_cast<size_t>(i))->descriptorInfo());
		}

		return buffersInfo;
	}

	void EngineSamplerDataStorageBuffer::createBuffers(std::shared_ptr<std::vector<SamplerData>> datas) {
		auto bufferSize = static_cast<VkDeviceSize>(sizeof(SamplerData));
		auto instanceCount = static_cast<uint32_t>(datas->size());
		auto totalSize = static_cast<VkDeviceSize>(bufferSize * instanceCount);
		
		this->buffers.clear();

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

			auto buffer = std::make_shared<EngineBuffer>(
				this->engineDevice,
				bufferSize,
				instanceCount,
				VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			);

			buffer->copyBuffer(stagingBuffer.getBuffer(), totalSize);
			this->buffers.emplace_back(buffer);
		}
	}

	void EngineSamplerDataStorageBuffer::transferToRead(std::shared_ptr<EngineCommandBuffer> commandBuffer, uint32_t frameIndex) {
		this->buffers.at(static_cast<size_t>(frameIndex))->transitionBuffer(VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, 
			VK_ACCESS_SHADER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, commandBuffer);
	}

	void EngineSamplerDataStorageBuffer::transferToWrite(std::shared_ptr<EngineCommandBuffer> commandBuffer, uint32_t frameIndex) {
		this->buffers.at(static_cast<size_t>(frameIndex))->transitionBuffer(VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, 
			VK_ACCESS_SHADER_READ_BIT, VK_ACCESS_SHADER_WRITE_BIT, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, commandBuffer);
	} 

	void EngineSamplerDataStorageBuffer::transferFromReadToWriteRead(std::shared_ptr<EngineCommandBuffer> commandBuffer, uint32_t frameIndex) {
		this->buffers.at(static_cast<size_t>(frameIndex))->transitionBuffer(VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, 
			VK_ACCESS_SHADER_READ_BIT, VK_ACCESS_SHADER_WRITE_BIT | VK_ACCESS_SHADER_READ_BIT, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, commandBuffer);
	}

	void EngineSamplerDataStorageBuffer::transferFromWriteReadToRead(std::shared_ptr<EngineCommandBuffer> commandBuffer, uint32_t frameIndex) {
		this->buffers.at(static_cast<size_t>(frameIndex))->transitionBuffer(VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, 
			VK_ACCESS_SHADER_WRITE_BIT | VK_ACCESS_SHADER_READ_BIT, VK_ACCESS_SHADER_READ_BIT, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, commandBuffer);
	}
} // namespace nugiEngine

