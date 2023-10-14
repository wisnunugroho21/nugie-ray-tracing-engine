#pragma once

#include <string>
#include <vector>
#include <memory>

#include "../device/device.hpp"

namespace NugieVulkan {
	class ComputePipeline {
		public:
			class Builder {
				public:
					Builder(Device* device, VkPipelineLayout pipelineLayout);
					Builder& setDefault(const std::string& compFilePath);
					Builder& setShaderStageInfo(VkPipelineShaderStageCreateInfo shaderStagesInfo);
          Builder& setBasePipelineHandleInfo(VkPipeline basePipeline);
          Builder& setBasePipelineIndex(int32_t basePipelineIndex);

					std::unique_ptr<ComputePipeline> build();

				private:
					VkPipelineLayout pipelineLayout = nullptr;
					VkPipelineShaderStageCreateInfo shaderStageInfo{};
					VkPipeline basePipelineHandleInfo{};
					int32_t basePipelineIndex;

					Device* device;
			};

			ComputePipeline(Device* device, VkPipelineLayout pipelineLayout, 
				VkPipelineShaderStageCreateInfo shaderStageInfo, 
				VkPipeline basePipelineHandleInfo, int32_t basePipelineIndex);
			~ComputePipeline();

			void bind(VkCommandBuffer commandBuffer);
			void dispatch(VkCommandBuffer commandBuffer, uint32_t xSize, uint32_t ySize, uint32_t zSize);

		private:
			Device* device;
			VkPipeline computePipeline;
      VkShaderModule shaderModule{};

			void createGraphicPipeline(VkPipelineLayout pipelineLayout, 
				VkPipelineShaderStageCreateInfo shaderStageInfo, 
				VkPipeline basePipelineHandleInfo, int32_t basePipelineIndex);
			
			static std::vector<char> readFile(const std::string& filepath);
			static void createShaderModule(Device* device, const std::vector<char>& code, VkShaderModule* shaderModule);
	};
}