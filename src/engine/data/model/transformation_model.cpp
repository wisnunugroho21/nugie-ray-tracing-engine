#include "transformation_model.hpp"

#include <cstring>
#include <iostream>
#include <unordered_map>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

namespace NugieApp {
	TransformationModel::TransformationModel(NugieVulkan::Device* device, NugieVulkan::CommandBuffer *commandBuffer, std::vector<Transformation> transformations) : device{device} {
		this->createBuffers(commandBuffer, transformations);
	}

	TransformationModel::TransformationModel(NugieVulkan::Device* device, NugieVulkan::CommandBuffer *commandBuffer, std::vector<TransformComponent> transformationComponents) : device{device} {
		this->createBuffers(commandBuffer, this->convertToMatrix(transformationComponents));
	}

	std::vector<Transformation> TransformationModel::convertToMatrix(std::vector<TransformComponent> transformations) {
		auto newTransforms = std::vector<Transformation>();
		for (auto &&transform : transformations) {
			newTransforms.emplace_back(Transformation{ 
				transform.getPointMatrix(),
				transform.getDirMatrix(), 
				transform.getPointInverseMatrix(), 
				transform.getDirInverseMatrix(), 
				transform.getNormalMatrix() 
			});
		}

		return newTransforms;
	}

	void TransformationModel::createBuffers(NugieVulkan::CommandBuffer *commandBuffer, std::vector<Transformation> transformations) {
		auto bufferSize = static_cast<VkDeviceSize>(sizeof(Transformation));
		auto instanceCount = static_cast<uint32_t>(transformations.size());

		NugieVulkan::Buffer transformationStagingBuffer {
			this->device,
			bufferSize,
			instanceCount,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};

		transformationStagingBuffer.map();
		transformationStagingBuffer.writeToBuffer(transformations.data());

		this->transformationBuffer = std::make_shared<NugieVulkan::Buffer>(
			this->device,
			bufferSize,
			instanceCount,
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		this->transformationBuffer->copyFromAnotherBuffer(&transformationStagingBuffer, commandBuffer);
	} 
} // namespace NugieApp

