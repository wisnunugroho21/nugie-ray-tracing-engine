#pragma once

#include "../../../vulkan/device/device.hpp"
#include "../../../vulkan/buffer/buffer.hpp"
#include "../../../vulkan/descriptor/descriptor_pool.hpp"
#include "../../../vulkan/image/image.hpp"

#include <memory>

namespace NugieApp {
	class RayTraceImage {
		public:
			RayTraceImage(NugieVulkan::Device* device, uint32_t width, uint32_t height, uint32_t imageCount);

			std::vector<VkDescriptorImageInfo> getImagesInfo();
      
      void prepareFrame(NugieVulkan::CommandBuffer* commandBuffer, uint32_t frameIndex);
			void transferFrame(NugieVulkan::CommandBuffer* commandBuffer, uint32_t frameIndex);
			void finishFrame(NugieVulkan::CommandBuffer* commandBuffer, uint32_t frameIndex);

		private:
			std::vector<NugieVulkan::Image> rayTraceImages;

      void createRayTraceImages(NugieVulkan::Device* device, uint32_t width, uint32_t height, uint32_t imageCount);
	};
	
}