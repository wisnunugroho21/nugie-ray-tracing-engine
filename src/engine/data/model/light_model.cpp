#include "light_model.hpp"

#include <cstring>
#include <iostream>
#include <unordered_map>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

namespace NugieApp {
	LightModel::LightModel(NugieVulkan::Device* device, NugieVulkan::CommandBuffer *commandBuffer, std::vector<TriangleLight> triangleLights, std::vector<RayTraceVertex> vertices) : device{device} {
		std::vector<BoundBox*> boundBoxes;
		for (int i = 0; i < triangleLights.size(); i++) {
			boundBoxes.push_back(new TriangleLightBoundBox(i + 1, &triangleLights[i], vertices));
		}

		this->createBuffers(commandBuffer, triangleLights, createBvh(boundBoxes));

		for (auto &&boundBox : boundBoxes) {
			delete boundBox;
		}
	}

	void LightModel::createBuffers(NugieVulkan::CommandBuffer *commandBuffer, std::vector<TriangleLight> triangleLights, std::vector<BvhNode> bvhNodes) {
		auto bufferSize = static_cast<VkDeviceSize>(sizeof(TriangleLight));
		auto instanceCount = static_cast<uint32_t>(triangleLights.size());
		
		NugieVulkan::Buffer lightStagingBuffer {
			this->device,
			bufferSize,
			instanceCount,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};

		lightStagingBuffer.map();
		lightStagingBuffer.writeToBuffer(triangleLights.data());

		this->lightBuffer = std::make_shared<NugieVulkan::Buffer>(
			this->device,
			bufferSize,
			instanceCount,
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		this->lightBuffer->copyFromAnotherBuffer(&lightStagingBuffer, commandBuffer);

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

		this->bvhBuffer = std::make_shared<NugieVulkan::Buffer>(
			this->device,
			bufferSize,
			instanceCount,
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		this->bvhBuffer->copyFromAnotherBuffer(&bvhStagingBuffer, commandBuffer);
	}
    
} // namespace NugieApp

