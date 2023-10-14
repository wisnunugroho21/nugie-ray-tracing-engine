#include "object_model.hpp"

#include <cstring>
#include <iostream>
#include <unordered_map>

namespace NugieApp {
	ObjectModel::ObjectModel(NugieVulkan::Device* device, std::vector<Object> objects, std::vector<BoundBox*> boundBoxes) : device{device} {
		this->createBuffers(objects, createBvh(boundBoxes));
	}

	void ObjectModel::createBuffers(std::vector<Object> objects, std::vector<BvhNode> bvhNodes) {
		auto bufferSize = static_cast<VkDeviceSize>(sizeof(Object));
		auto instanceCount = static_cast<uint32_t>(objects.size());

		NugieVulkan::Buffer objectStagingBuffer {
			this->device,
			bufferSize,
			instanceCount,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};

		objectStagingBuffer.map();
		objectStagingBuffer.writeToBuffer(objects.data());

		this->objectBuffer = std::make_unique<NugieVulkan::Buffer>(
			this->device,
			bufferSize,
			instanceCount,
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		this->objectBuffer->copyFromAnotherBuffer(&objectStagingBuffer);

		// -------------------------------------------------

		bufferSize = static_cast<VkDeviceSize>(sizeof(BvhNode));
		instanceCount = static_cast<uint32_t>(bvhNodes.size());

		NugieVulkan::Buffer bvhStagingBuffer {
			this->device,
			bufferSize,
			instanceCount,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};

		bvhStagingBuffer.map();
		bvhStagingBuffer.writeToBuffer(bvhNodes.data());

		this->bvhBuffer = std::make_unique<NugieVulkan::Buffer>(
			this->device,
			bufferSize,
			instanceCount,
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		this->bvhBuffer->copyFromAnotherBuffer(&bvhStagingBuffer);
	} 
} // namespace NugieApp

