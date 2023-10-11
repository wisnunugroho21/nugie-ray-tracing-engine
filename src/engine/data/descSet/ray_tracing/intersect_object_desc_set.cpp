#include "intersect_object_desc_set.hpp"

namespace nugiEngine {
  EngineIntersectObjectDescSet::EngineIntersectObjectDescSet(EngineDevice& device, std::shared_ptr<EngineDescriptorPool> descriptorPool,
		std::vector<VkDescriptorBufferInfo> buffersInfo[2], VkDescriptorBufferInfo modelsInfo[7]) 
	{
		this->createDescriptor(device, descriptorPool, buffersInfo, modelsInfo);
  }

  void EngineIntersectObjectDescSet::createDescriptor(EngineDevice& device, std::shared_ptr<EngineDescriptorPool> descriptorPool,
	std::vector<VkDescriptorBufferInfo> buffersInfo[2], VkDescriptorBufferInfo modelsInfo[7]) 
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
				.addBinding(7, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
				.addBinding(8, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
				.build();
		
	this->descriptorSets.clear();
		for (int i = 0; i < EngineDevice::MAX_FRAMES_IN_FLIGHT; i++) {
			VkDescriptorSet descSet;

			auto y = EngineDescriptorWriter(*this->descSetLayout, *descriptorPool)
				.writeBuffer(0, &buffersInfo[0][i])
				.writeBuffer(1, &buffersInfo[1][i])
				.writeBuffer(2, &modelsInfo[0])
				.writeBuffer(3, &modelsInfo[1])
				.writeBuffer(4, &modelsInfo[2])
				.writeBuffer(5, &modelsInfo[3])
				.writeBuffer(6, &modelsInfo[4])
				.writeBuffer(7, &modelsInfo[5])
				.writeBuffer(8, &modelsInfo[6])
				.build(&descSet);

			this->descriptorSets.emplace_back(descSet);
		}
  }
}