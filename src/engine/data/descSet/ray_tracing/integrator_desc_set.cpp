#include "integrator_desc_set.hpp"

namespace nugiEngine {
  EngineIntegratorDescSet::EngineIntegratorDescSet(EngineDevice& device, std::shared_ptr<EngineDescriptorPool> descriptorPool,
		std::vector<VkDescriptorImageInfo> indirectImageInfos, std::vector<VkDescriptorBufferInfo> buffersInfo[7]) 
	{
		this->createDescriptor(device, descriptorPool, indirectImageInfos, buffersInfo);
  }

  void EngineIntegratorDescSet::createDescriptor(EngineDevice& device, std::shared_ptr<EngineDescriptorPool> descriptorPool,
		std::vector<VkDescriptorImageInfo> indirectImageInfos, std::vector<VkDescriptorBufferInfo> buffersInfo[7]) 
	{
    this->descSetLayout = 
			EngineDescriptorSetLayout::Builder(device)
				.addBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_COMPUTE_BIT)
				.addBinding(1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
				.addBinding(2, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
				.addBinding(3, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
				.addBinding(4, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
				.addBinding(5, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
				.addBinding(6, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
				.addBinding(7, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
				.build();
		
	this->descriptorSets.clear();
		for (int i = 0; i < EngineDevice::MAX_FRAMES_IN_FLIGHT; i++) {
			VkDescriptorSet descSet{};

			EngineDescriptorWriter(*this->descSetLayout, *descriptorPool)
				.writeImage(0, &indirectImageInfos[i])
				.writeBuffer(1, &buffersInfo[0][i])
				.writeBuffer(2, &buffersInfo[1][i])
				.writeBuffer(3, &buffersInfo[2][i])
				.writeBuffer(4, &buffersInfo[3][i])
				.writeBuffer(5, &buffersInfo[4][i])
				.writeBuffer(6, &buffersInfo[5][i])
				.writeBuffer(7, &buffersInfo[6][i])
				.build(&descSet);

			this->descriptorSets.emplace_back(descSet);
		}
  }
}