#pragma once

#include "pgs_device.hpp"

// std
#include <string>
#include <vector>

namespace pgs
{

struct PipelineConfigInfo
{
	PipelineConfigInfo(const PipelineConfigInfo &) = delete;
	PipelineConfigInfo &operator=(const PipelineConfigInfo &) = delete;

	VkPipelineViewportStateCreateInfo viewportInfo;
	VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
	VkPipelineRasterizationStateCreateInfo rasterizationInfo;
	VkPipelineMultisampleStateCreateInfo multisampleInfo;
	VkPipelineColorBlendAttachmentState colorBlendAttachment;
	VkPipelineColorBlendStateCreateInfo colorBlendInfo;
	VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
	std::vector<VkDynamicState> dynamicStateEnables;
	VkPipelineDynamicStateCreateInfo dynamicStateInfo;
	VkPipelineLayout pipelineLayout = nullptr;
	VkRenderPass renderPass = nullptr;
	uint32_t subpass = 0;
};

class PgsGraphicsPipeline
{
  public:
	PgsGraphicsPipeline(PgsDevice &device,
				const std::string &vertFilepath,
				const std::string &fragFilepath,
				const PipelineConfigInfo &configInfo);
	~PgsGraphicsPipeline();

	PgsGraphicsPipeline(const PgsGraphicsPipeline &) = delete;
	PgsGraphicsPipeline &operator=(const PgsGraphicsPipeline &) = delete;

	void bind(VkCommandBuffer commandBuffer);

	static void graphicsPipelineConfigInfo(PipelineConfigInfo &configInfo);

  private:
	void createGraphicsPipeline(const std::string &vertFilepath,
								const std::string &fragFilepath,
								const PipelineConfigInfo &configInfo);

	void createShaderModule(const std::vector<char> &code, VkShaderModule *shaderModule);

	PgsDevice &m_pgsDevice;
	VkPipeline m_graphicsPipeline;
	VkShaderModule m_vertShaderModule;
	VkShaderModule m_fragShaderModule;
};
} // namespace pgs
