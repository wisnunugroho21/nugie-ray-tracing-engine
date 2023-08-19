#include "direct_lambert_desc_set.hpp"

namespace nugiEngine {
  EngineDirectLambertDescSet::EngineDirectLambertDescSet(EngineDevice& device, std::shared_ptr<EngineDescriptorPool> descriptorPool,
		std::vector<VkDescriptorBufferInfo> buffersInfo[4], VkDescriptorBufferInfo modelsInfo[3], std::vector<VkDescriptorImageInfo> texturesInfo[1]) 
	{
		this->createDescriptor(device, descriptorPool, buffersInfo, modelsInfo, texturesInfo);
  }

  void EngineDirectLambertDescSet::createDescriptor(EngineDevice& device, std::shared_ptr<EngineDescriptorPool> descriptorPool,
		std::vector<VkDescriptorBufferInfo> buffersInfo[4], VkDescriptorBufferInfo modelsInfo[3], std::vector<VkDescriptorImageInfo> texturesInfo[1]) 
	{
    this->descSetLayout = 
			EngineDescriptorSetLayout::Builder(device)
				.addBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
				.addBinding(1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
				.addBinding(2, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
				.addBinding(3, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
				.addBinding(4, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
				.addBinding(5, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
				.addBinding(6, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
				.addBinding(7, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_COMPUTE_BIT, static_cast<uint32_t>(texturesInfo[0].size()))
				.build();
		
	this->descriptorSets.clear();
		for (int i = 0; i < EngineDevice::MAX_FRAMES_IN_FLIGHT; i++) {
			VkDescriptorSet descSet{};

			EngineDescriptorWriter(*this->descSetLayout, *descriptorPool)
				.writeBuffer(0, &buffersInfo[0][i])
				.writeBuffer(1, &buffersInfo[1][i])
				.writeBuffer(2, &buffersInfo[2][i])
				.writeBuffer(3, &buffersInfo[3][i])
				.writeBuffer(4, &modelsInfo[0])
				.writeBuffer(5, &modelsInfo[1])
				.writeBuffer(6, &modelsInfo[2])
				.writeImage(7, texturesInfo[0].data(), static_cast<uint32_t>(texturesInfo[0].size()))
				.build(&descSet);

			this->descriptorSets.emplace_back(descSet);
		}
  }
}