#include "object_model.hpp"

#include <cstring>
#include <iostream>
#include <unordered_map>

namespace nugiEngine {
	EngineObjectModel::EngineObjectModel(EngineDevice &device, std::shared_ptr<std::vector<Object>> objects, std::vector<std::shared_ptr<BoundBox>> boundBoxes) : engineDevice{device} {
		this->createBuffers(objects, createBvh(boundBoxes));
	}

	void EngineObjectModel::createBuffers(std::shared_ptr<std::vector<Object>> objects, std::shared_ptr<std::vector<BvhNode>> bvhNodes) {
		auto bufferSize = static_cast<VkDeviceSize>(sizeof(Object));
		auto instanceCount = static_cast<uint32_t>(objects->size());
		auto totalSize = static_cast<VkDeviceSize>(bufferSize * instanceCount);

		EngineBuffer objectStagingBuffer {
			this->engineDevice,
			bufferSize,
			instanceCount,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};

		objectStagingBuffer.map();
		objectStagingBuffer.writeToBuffer(objects->data());

		this->objectBuffer = std::make_shared<EngineBuffer>(
			this->engineDevice,
			bufferSize,
			instanceCount,
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		this->objectBuffer->copyBuffer(objectStagingBuffer.getBuffer(), totalSize);

		// -------------------------------------------------

		bufferSize = static_cast<VkDeviceSize>(sizeof(BvhNode));
		instanceCount = static_cast<uint32_t>(bvhNodes->size());
		totalSize = static_cast<VkDeviceSize>(bufferSize * instanceCount);

		EngineBuffer bvhStagingBuffer {
			this->engineDevice,
			bufferSize,
			instanceCount,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};

		bvhStagingBuffer.map();
		bvhStagingBuffer.writeToBuffer(bvhNodes->data());

		this->bvhBuffer = std::make_shared<EngineBuffer>(
			this->engineDevice,
			bufferSize,
			instanceCount,
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		this->bvhBuffer->copyBuffer(bvhStagingBuffer.getBuffer(), totalSize);
	} 
} // namespace nugiEngine

