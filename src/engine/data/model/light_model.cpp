#include "light_model.hpp"

#include <cstring>
#include <iostream>
#include <unordered_map>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

namespace nugiEngine {
	EngineLightModel::EngineLightModel(EngineDevice &device, std::shared_ptr<std::vector<TriangleLight>> triangleLights, std::shared_ptr<std::vector<RayTraceVertex>> vertices) : engineDevice{device} {
		std::vector<std::shared_ptr<BoundBox>> boundBoxes;
		for (int i = 0; i < triangleLights->size(); i++) {
			boundBoxes.push_back(std::make_shared<LightBoundBox>(LightBoundBox{ i + 1, (*triangleLights)[i], vertices }));
		}

		this->createBuffers(triangleLights, createBvh(boundBoxes));
	}

	void EngineLightModel::createBuffers(std::shared_ptr<std::vector<TriangleLight>> triangleLights, std::shared_ptr<std::vector<BvhNode>> bvhNodes) {
		auto lightBufferSize = sizeof(TriangleLight) * triangleLights->size();
		
		EngineBuffer lightStagingBuffer {
			this->engineDevice,
			static_cast<VkDeviceSize>(lightBufferSize),
			1,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};

		lightStagingBuffer.map();
		lightStagingBuffer.writeToBuffer(triangleLights->data());

		this->lightBuffer = std::make_shared<EngineBuffer>(
			this->engineDevice,
			static_cast<VkDeviceSize>(lightBufferSize),
			1,
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		this->lightBuffer->copyBuffer(lightStagingBuffer.getBuffer(), static_cast<VkDeviceSize>(lightBufferSize));

		// -------------------------------------------------

		auto bvhBufferSize = sizeof(BvhNode) * bvhNodes->size();

		EngineBuffer bvhStagingBuffer {
			this->engineDevice,
			static_cast<VkDeviceSize>(bvhBufferSize),
			1,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};

		bvhStagingBuffer.map();
		bvhStagingBuffer.writeToBuffer(bvhNodes->data());

		this->bvhBuffer = std::make_shared<EngineBuffer>(
			this->engineDevice,
			static_cast<VkDeviceSize>(bvhBufferSize),
			1,
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		this->bvhBuffer->copyBuffer(bvhStagingBuffer.getBuffer(), static_cast<VkDeviceSize>(bvhBufferSize));
	}
    
} // namespace nugiEngine

