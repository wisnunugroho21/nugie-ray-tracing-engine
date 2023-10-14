#pragma once

#include "../../../vulkan/device/device.hpp"
#include "../../../vulkan/buffer/buffer.hpp"
#include "../../../vulkan/command/command_buffer.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>
#include <memory>

namespace NugieApp {
	struct Vertex {
		glm::vec3 position{};

		static std::vector<VkVertexInputBindingDescription> getVertexBindingDescriptions();
		static std::vector<VkVertexInputAttributeDescription> getVertexAttributeDescriptions();

		bool operator == (const Vertex &other) const {
			return this->position == other.position;
		}
	};

	struct VertexModelData
	{
		std::vector<Vertex> vertices{};
		std::vector<uint32_t> indices{};
	};

	class VertexModel {
	public:
		VertexModel(NugieVulkan::Device* device, const VertexModelData &data);

		void bind(NugieVulkan::CommandBuffer* commandBuffer);
		void draw(NugieVulkan::CommandBuffer* commandBuffer);
		
	private:
		NugieVulkan::Device* device;
		
		std::unique_ptr<NugieVulkan::Buffer> vertexBuffer;
		uint32_t vertextCount;

		std::unique_ptr<NugieVulkan::Buffer> indexBuffer;
		uint32_t indexCount;

		bool hasIndexBuffer = false;

		void createVertexBuffers(const std::vector<Vertex> &vertices);
		void createIndexBuffer(const std::vector<uint32_t> &indices);
	};
} // namespace NugieApp
