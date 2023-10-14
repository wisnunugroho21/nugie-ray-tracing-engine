#include "accumulate_image.hpp"

namespace NugieApp {
  AccumulateImage::AccumulateImage(NugieVulkan::Device* device, uint32_t width, uint32_t height, uint32_t imageCount) {
		this->createAccumulateImages(device, width, height, imageCount);
  }

	std::vector<VkDescriptorImageInfo> AccumulateImage::getImagesInfo() {
		std::vector<VkDescriptorImageInfo> imagesInfo{};

		for (auto &&accumulateImage : this->accumulateImages) {
			imagesInfo.emplace_back(accumulateImage.getDescriptorInfo(VK_IMAGE_LAYOUT_GENERAL));
		}

		return imagesInfo;
	}

	void AccumulateImage::createAccumulateImages(NugieVulkan::Device* device, uint32_t width, uint32_t height, uint32_t imageCount) {
   	this->accumulateImages.clear();

		for (uint32_t i = 0; i < imageCount; i++) {
			auto accumulateImage = NugieVulkan::Image(
				device, width, height, 
				1, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_B8G8R8A8_UNORM, 
				VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_STORAGE_BIT, 
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT
			);

			this->accumulateImages.emplace_back(accumulateImage);
		}
  }

	void AccumulateImage::prepareFrame(NugieVulkan::CommandBuffer* commandBuffer, uint32_t frameIndex) {
		if (this->accumulateImages[frameIndex].getLayout() == VK_IMAGE_LAYOUT_UNDEFINED) {
			this->accumulateImages[frameIndex].transitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL,
				VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 
				0, VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT, 
				VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, commandBuffer);
		} else {
			this->accumulateImages[frameIndex].transitionImageLayout(VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_GENERAL,
				VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
				0, VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT,
				VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, commandBuffer);
		}
	}

	void AccumulateImage::finishFrame(NugieVulkan::CommandBuffer* commandBuffer, uint32_t frameIndex) {
		this->accumulateImages[frameIndex].transitionImageLayout(VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_GENERAL,
			VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
			VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT, 0,
			VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, commandBuffer);
	}
}