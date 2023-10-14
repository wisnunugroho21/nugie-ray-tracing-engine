#include "material_model.hpp"

#include <cstring>
#include <iostream>
#include <unordered_map>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

namespace NugieApp {
	MaterialModel::MaterialModel(NugieVulkan::Device* device, std::vector<Material> materials) : device{device} {
		this->createBuffers(materials);
	}

	void MaterialModel::createBuffers(std::vector<Material> materials) {
		auto bufferSize = static_cast<VkDeviceSize>(sizeof(Material));
		auto instanceCount = static_cast<uint32_t>(materials.size());

		NugieVulkan::Buffer materialStagingBuffer {
			this->device,
			bufferSize,
			instanceCount,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};

		materialStagingBuffer.map();
		materialStagingBuffer.writeToBuffer(materials.data());

		this->materialBuffer = std::make_unique<NugieVulkan::Buffer>(
			this->device,
			bufferSize,
			instanceCount,
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		this->materialBuffer->copyFromAnotherBuffer(&materialStagingBuffer);
	} 
} // namespace NugieApp

