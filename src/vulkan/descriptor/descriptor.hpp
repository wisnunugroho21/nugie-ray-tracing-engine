#pragma once
 
#include "../device/device.hpp"
 
// std
#include <memory>
#include <unordered_map>
#include <vector>
 
namespace nugiEngine {
 
class EngineDescriptorSetLayout {
 public:
  class Builder {
   public:
    Builder(EngineDevice &engineDevice) : engineDevice{engineDevice} {}
 
    Builder &addBinding(
      uint32_t binding,
      VkDescriptorType descriptorType,
      VkShaderStageFlags stageFlags,
      uint32_t count = 1
    );
    std::shared_ptr<EngineDescriptorSetLayout> build() const;
 
   private:
    EngineDevice &engineDevice;
    std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
  };
 
  EngineDescriptorSetLayout(EngineDevice &engineDevice, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
  ~EngineDescriptorSetLayout();
 
  VkDescriptorSetLayout getDescriptorSetLayout() const { return this->descriptorSetLayout; }
 
 private:
  EngineDevice &engineDevice;
  VkDescriptorSetLayout descriptorSetLayout;
  std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;
 
  friend class EngineDescriptorWriter;
};
 
class EngineDescriptorPool {
 public:
  class Builder {
   public:
    Builder(EngineDevice &engineDevice) : engineDevice{engineDevice} {}
 
    Builder &addPoolSize(VkDescriptorType descriptorType, uint32_t count);
    Builder &setPoolFlags(VkDescriptorPoolCreateFlags flags);
    Builder &setMaxSets(uint32_t count);
    std::shared_ptr<EngineDescriptorPool> build() const;
 
   private:
    EngineDevice &engineDevice;
    std::vector<VkDescriptorPoolSize> poolSizes{};
    uint32_t maxSets = 1000;
    VkDescriptorPoolCreateFlags poolFlags = 0;
  };
 
  EngineDescriptorPool(
    EngineDevice &engineDevice,
    uint32_t maxSets,
    VkDescriptorPoolCreateFlags poolFlags,
    const std::vector<VkDescriptorPoolSize> &poolSizes
  );
  ~EngineDescriptorPool();
 
  bool allocateDescriptor(const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet *descriptor) const;
  void freeDescriptors(std::vector<VkDescriptorSet> &descriptors) const;
  void resetPool();
 
 private:
  EngineDevice &engineDevice;
  VkDescriptorPool descriptorPool;
 
  friend class EngineDescriptorWriter;
};
 
class EngineDescriptorWriter {
 public:
  EngineDescriptorWriter(EngineDescriptorSetLayout &setLayout, EngineDescriptorPool &pool);
 
  EngineDescriptorWriter &writeBuffer(uint32_t binding, VkDescriptorBufferInfo *bufferInfo, uint32_t count = 1);
  EngineDescriptorWriter &writeImage(uint32_t binding, VkDescriptorImageInfo *imageInfo, uint32_t count = 1);
 
  bool build(VkDescriptorSet *set);
  void overwrite(VkDescriptorSet *set);
 
 private:
  EngineDescriptorSetLayout &setLayout;
  EngineDescriptorPool &pool;
  std::vector<VkWriteDescriptorSet> writes;
};
 
}  // namespace lve