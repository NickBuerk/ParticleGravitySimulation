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

class pgsPipeline
{
  public:
	pgsPipeline(PgsDevice &device,
				const std::string &vertFilepath,
				const std::string &fragFilepath,
				const PipelineConfigInfo &configInfo);
	~pgsPipeline();

	pgsPipeline(const pgsPipeline &) = delete;
	pgsPipeline &operator=(const pgsPipeline &) = delete;

	void bind(VkCommandBuffer commandBuffer);

	static void defaultPipelineConfigInfo(PipelineConfigInfo &configInfo);

  private:
	static std::vector<char> readFile(const std::string &filepath);

	void createGraphicsPipeline(const std::string &vertFilepath,
								const std::string &fragFilepath,
								const PipelineConfigInfo &configInfo);

	void createShaderModule(const std::vector<char> &code, VkShaderModule *shaderModule);

	PgsDevice &pgsDevice;
	VkPipeline graphicsPipeline;
	VkShaderModule vertShaderModule;
	VkShaderModule fragShaderModule;
};
} // namespace pgs
