#include "morton_model.hpp"

namespace nugiEngine {
	EngineMortonModel::EngineMortonModel(EngineDevice &device, std::shared_ptr<std::vector<Pixel>> mortonPixels) : engineDevice{device} {
		this->createBuffers(mortonPixels);
	}

	void EngineMortonModel::createBuffers(std::shared_ptr<std::vector<Pixel>> mortonPixels) {
		auto mortonPixelBufferSize = sizeof(Pixel) * mortonPixels->size();

		EngineBuffer mortonPixelStagingBuffer {
			this->engineDevice,
			static_cast<VkDeviceSize>(mortonPixelBufferSize),
			1,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};

		mortonPixelStagingBuffer.map();
		mortonPixelStagingBuffer.writeToBuffer(mortonPixels->data());

		this->mortonPixelBuffer = std::make_shared<EngineBuffer>(
			this->engineDevice,
			static_cast<VkDeviceSize>(mortonPixelBufferSize),
			1,
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		this->mortonPixelBuffer->copyBuffer(mortonPixelStagingBuffer.getBuffer(), static_cast<VkDeviceSize>(mortonPixelBufferSize));
	} 
} // namespace nugiEngine

