#include "graphic_pipeline.hpp"

#include <fstream>
#include <iostream>
#include <stdexcept>

namespace NugieVulkan {
	GraphicPipeline::Builder::Builder(Device* device, RenderPass renderPass, VkPipelineLayout pipelineLayout) : device{device} {
		this->pipelineLayout = pipelineLayout;
		this->renderPass = renderPass.getRenderPass();
	}

	GraphicPipeline::Builder& GraphicPipeline::Builder::setDefault(
		const std::string& vertFilePath, 
		const std::string& fragFilePath, 
		std::vector<VkVertexInputBindingDescription> bindingDescriptions,
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions) 
	{
		auto msaaSamples = this->device->getMSAASamples();

		this->inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		this->inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		this->inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;
		
		this->rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		this->rasterizationInfo.depthClampEnable = VK_FALSE;
		this->rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
		this->rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
		this->rasterizationInfo.lineWidth = 1.0f;
		this->rasterizationInfo.cullMode = VK_CULL_MODE_NONE;
		this->rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
		this->rasterizationInfo.depthBiasEnable = VK_FALSE;
		this->rasterizationInfo.depthBiasConstantFactor = 0.0f;  // Optional
		this->rasterizationInfo.depthBiasClamp = 0.0f;           // Optional
		this->rasterizationInfo.depthBiasSlopeFactor = 0.0f;     // Optional
		
		this->multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		this->multisampleInfo.sampleShadingEnable = VK_TRUE;
		this->multisampleInfo.rasterizationSamples = msaaSamples;
		this->multisampleInfo.minSampleShading = 0.2f;           
		this->multisampleInfo.pSampleMask = nullptr;             // Optional
		this->multisampleInfo.alphaToCoverageEnable = VK_FALSE;  // Optional
		this->multisampleInfo.alphaToOneEnable = VK_FALSE;       // Optional
		
		this->colorBlendAttachment.colorWriteMask =
			VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		this->colorBlendAttachment.blendEnable = VK_FALSE;
		this->colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
		this->colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
		this->colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;              // Optional
		this->colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
		this->colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
		this->colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;              // Optional
		
		this->colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		this->colorBlendInfo.logicOpEnable = VK_FALSE;
		this->colorBlendInfo.logicOp = VK_LOGIC_OP_COPY;  // Optional
		this->colorBlendInfo.attachmentCount = 1;
		this->colorBlendInfo.pAttachments = &this->colorBlendAttachment;
		this->colorBlendInfo.blendConstants[0] = 0.0f;  // Optional
		this->colorBlendInfo.blendConstants[1] = 0.0f;  // Optional
		this->colorBlendInfo.blendConstants[2] = 0.0f;  // Optional
		this->colorBlendInfo.blendConstants[3] = 0.0f;  // Optional
		
		this->depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		this->depthStencilInfo.depthTestEnable = VK_TRUE;
		this->depthStencilInfo.depthWriteEnable = VK_TRUE;
		this->depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
		this->depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
		this->depthStencilInfo.minDepthBounds = 0.0f;  // Optional
		this->depthStencilInfo.maxDepthBounds = 1.0f;  // Optional
		this->depthStencilInfo.stencilTestEnable = VK_FALSE;
		this->depthStencilInfo.front = {};  // Optional
		this->depthStencilInfo.back = {};   // Optional

		this->dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

		this->dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		this->dynamicStateInfo.pDynamicStates = this->dynamicStates.data();
		this->dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(this->dynamicStates.size());
		this->dynamicStateInfo.flags = 0;

		this->bindingDescriptions = bindingDescriptions;
		this->attributeDescriptions = attributeDescriptions;

		VkShaderModule vertShaderModule;
		VkShaderModule fragShaderModule;

		auto vertCode = GraphicPipeline::readFile(vertFilePath);
		auto fragCode = GraphicPipeline::readFile(fragFilePath);

		GraphicPipeline::createShaderModule(this->device, vertCode, &vertShaderModule);
		GraphicPipeline::createShaderModule(this->device, fragCode, &fragShaderModule);

		VkPipelineShaderStageCreateInfo vertexShaderStageInfo{};
		vertexShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertexShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertexShaderStageInfo.module = vertShaderModule;
		vertexShaderStageInfo.pName = "main";
		vertexShaderStageInfo.flags = 0;
		vertexShaderStageInfo.pNext = nullptr;
		vertexShaderStageInfo.pSpecializationInfo = nullptr;

		VkPipelineShaderStageCreateInfo fragmentShaderStageInfo{};
		fragmentShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragmentShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragmentShaderStageInfo.module = fragShaderModule;
		fragmentShaderStageInfo.pName = "main";
		fragmentShaderStageInfo.flags = 0;
		fragmentShaderStageInfo.pNext = nullptr;
		fragmentShaderStageInfo.pSpecializationInfo = nullptr;

		this->shaderStagesInfo = { vertexShaderStageInfo, fragmentShaderStageInfo };

		return *this;
	}

	GraphicPipeline::Builder& GraphicPipeline::Builder::setSubpass(uint32_t subpass) {
		this->subpass = subpass;
		return *this;
	}

	GraphicPipeline::Builder& GraphicPipeline::Builder::setBindingDescriptions(std::vector<VkVertexInputBindingDescription> bindingDescriptions) {
		this->bindingDescriptions = bindingDescriptions;
		return *this;
	}

	GraphicPipeline::Builder& GraphicPipeline::Builder::setAttributeDescriptions (std::vector<VkVertexInputAttributeDescription> attributeDescriptions) {
		this->attributeDescriptions = attributeDescriptions;
		return *this;
	}

	GraphicPipeline::Builder& GraphicPipeline::Builder::setInputAssemblyInfo(VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo) {
		this->inputAssemblyInfo = inputAssemblyInfo;
		return *this;
	}

	GraphicPipeline::Builder& GraphicPipeline::Builder::setRasterizationInfo(VkPipelineRasterizationStateCreateInfo rasterizationInfo) {
		this->rasterizationInfo = rasterizationInfo;
		return *this;
	}

	GraphicPipeline::Builder& GraphicPipeline::Builder::setMultisampleInfo(VkPipelineMultisampleStateCreateInfo multisampleInfo) {
		this->multisampleInfo = multisampleInfo;
		return *this;
	}

	GraphicPipeline::Builder& GraphicPipeline::Builder::setColorBlendAttachment(VkPipelineColorBlendAttachmentState colorBlendAttachment) {
		this->colorBlendAttachment = colorBlendAttachment;
		return *this;
	}

	GraphicPipeline::Builder& GraphicPipeline::Builder::setColorBlendInfo(VkPipelineColorBlendStateCreateInfo colorBlendInfo) {
		this->colorBlendInfo = colorBlendInfo;
		return *this;
	}

	GraphicPipeline::Builder& GraphicPipeline::Builder::setDepthStencilInfo(VkPipelineDepthStencilStateCreateInfo depthStencilInfo) {
		this->depthStencilInfo = depthStencilInfo;
		return *this;
	}

	GraphicPipeline::Builder& GraphicPipeline::Builder::setDynamicStateInfo(VkPipelineDynamicStateCreateInfo dynamicStateInfo) {
		this->dynamicStateInfo = dynamicStateInfo;
		return *this;
	}

	GraphicPipeline::Builder& GraphicPipeline::Builder::setShaderStagesInfo(std::vector<VkPipelineShaderStageCreateInfo> shaderStagesInfo) {
		this->shaderStagesInfo = shaderStagesInfo;
		return *this;
	}

	std::unique_ptr<GraphicPipeline> GraphicPipeline::Builder::build() {
		return std::make_unique<GraphicPipeline>(
			this->device,
			this->pipelineLayout,
			this->renderPass,
			this->subpass,
			this->bindingDescriptions,
			this->attributeDescriptions,
			this->inputAssemblyInfo,
			this->rasterizationInfo,
			this->multisampleInfo,
			this->colorBlendAttachment,
			this->colorBlendInfo,
			this->depthStencilInfo,
			this->dynamicStateInfo,
			this->shaderStagesInfo
		);
	}

	GraphicPipeline::GraphicPipeline(
		Device* device, 
		VkPipelineLayout pipelineLayout,
		VkRenderPass renderPass,
		uint32_t subpass,
		std::vector<VkVertexInputBindingDescription> bindingDescriptions,
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions,
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo,
		VkPipelineRasterizationStateCreateInfo rasterizationInfo,
		VkPipelineMultisampleStateCreateInfo multisampleInfo,
		VkPipelineColorBlendAttachmentState colorBlendAttachment,
		VkPipelineColorBlendStateCreateInfo colorBlendInfo,
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo,
		VkPipelineDynamicStateCreateInfo dynamicStateInfo,
		std::vector<VkPipelineShaderStageCreateInfo> shaderStagesInfo
	) : device{device} 
	{
		this->createGraphicPipeline(
			pipelineLayout,
			renderPass,
			subpass,
			bindingDescriptions,
			attributeDescriptions,
			inputAssemblyInfo,
			rasterizationInfo,
			multisampleInfo,
			colorBlendAttachment,
			colorBlendInfo,
			depthStencilInfo,
			dynamicStateInfo,
			shaderStagesInfo
		);
	}

	GraphicPipeline::~GraphicPipeline() {
		for (auto& shaderModule : this->shaderModules) {
			vkDestroyShaderModule(this->device->getLogicalDevice(), shaderModule, nullptr);
		}

		vkDestroyPipeline(this->device->getLogicalDevice(), this->graphicPipeline, nullptr);
	}

	std::vector<char> GraphicPipeline::readFile(const std::string& filepath) {
		std::ifstream file{filepath, std::ios::ate | std::ios::binary};

		if (!file.is_open()) {
			throw std::runtime_error("failed to open file");
		}

		size_t fileSize = static_cast<size_t>(file.tellg());
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);

		file.close();
		return buffer;
	}

	void GraphicPipeline::createGraphicPipeline(
		VkPipelineLayout pipelineLayout,
		VkRenderPass renderPass,
		uint32_t subpass,
		std::vector<VkVertexInputBindingDescription> bindingDescriptions,
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions,
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo,
		VkPipelineRasterizationStateCreateInfo rasterizationInfo,
		VkPipelineMultisampleStateCreateInfo multisampleInfo,
		VkPipelineColorBlendAttachmentState colorBlendAttachment,
		VkPipelineColorBlendStateCreateInfo colorBlendInfo,
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo,
		VkPipelineDynamicStateCreateInfo dynamicStateInfo,
		std::vector<VkPipelineShaderStageCreateInfo> shaderStagesInfo) 
	{
		auto bindingDescriptions = bindingDescriptions;
		auto attributeDescriptions = attributeDescriptions;

		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
		vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptions.size());
		vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
		vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();

		VkPipelineViewportStateCreateInfo viewportInfo{};
		viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportInfo.viewportCount = 1;
		viewportInfo.pViewports = nullptr;
		viewportInfo.scissorCount = 1;
		viewportInfo.pScissors = nullptr;

		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = static_cast<uint32_t>(shaderStagesInfo.size());
		pipelineInfo.pStages = shaderStagesInfo.data();
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssemblyInfo;
		pipelineInfo.pViewportState = &viewportInfo;
		pipelineInfo.pRasterizationState = &rasterizationInfo;
		pipelineInfo.pMultisampleState = &multisampleInfo;
		pipelineInfo.pColorBlendState = &colorBlendInfo;
		pipelineInfo.pDepthStencilState = &depthStencilInfo;
		pipelineInfo.pDynamicState = &dynamicStateInfo;

		pipelineInfo.layout = pipelineLayout;
		pipelineInfo.renderPass = renderPass;
		pipelineInfo.subpass = subpass;

		pipelineInfo.basePipelineIndex = -1;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

		if (vkCreateGraphicsPipelines(this->device->getLogicalDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &this->graphicPipeline) != VK_SUCCESS) {
			throw std::runtime_error("failed to create graphic pipelines");
		}
		
		this->shaderModules.clear();
		for (auto& shaderStage : shaderStagesInfo) {
			this->shaderModules.push_back(shaderStage.module);
		}
	}

	void GraphicPipeline::createShaderModule(Device* device, const std::vector<char>& code, VkShaderModule* shaderModule) {
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		if (vkCreateShaderModule(device->getLogicalDevice(), &createInfo, nullptr, shaderModule) != VK_SUCCESS) {
			throw std::runtime_error("failed to create shader module");
		}
	}

	void GraphicPipeline::bind(VkCommandBuffer commandBuffer) {
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, this->graphicPipeline);
	}
} // namespace NugieVulkan
