#include "pgs_computePipeline.hpp"
#include "pgs_graphicsPipeline.hpp"
#include "../pgs_utils.hpp"
#include "pgs_model.hpp"

// std
#include <cassert>
#include <iostream>
#include <stdexcept>

namespace pgs
{

PgsComputePipeline::PgsComputePipeline(PgsDevice &device,
						 const std::string &compFilepath,
						 const VkPipelineLayout &pipelineLayout)
	: m_pgsDevice{device}
{
	createComputePipeline(compFilepath, pipelineLayout);
}

PgsComputePipeline::~PgsComputePipeline()
{
	vkDestroyShaderModule(m_pgsDevice.device(), m_compShaderModule, nullptr);
	vkDestroyPipeline(m_pgsDevice.device(), m_computePipeline, nullptr);
}

void PgsComputePipeline::createComputePipeline(const std::string &compFilepath,
										 const VkPipelineLayout &pipelineLayout)
{
	assert(pipelineLayout != VK_NULL_HANDLE &&
		   "Cannot create graphics pipeline: no pipelineLayout provided in "
		   "configInfo");

	auto compCode = readShaderFile(compFilepath);
	createShaderModule(compCode, &m_compShaderModule);

	VkPipelineShaderStageCreateInfo shaderStage{};
	shaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
	shaderStage.module = m_compShaderModule;
	shaderStage.pName = "main";

	VkComputePipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	pipelineInfo.stage = shaderStage;
	pipelineInfo.layout = pipelineLayout;

	if (vkCreateComputePipelines(m_pgsDevice.device(),
								  VK_NULL_HANDLE,
								  1,
								  &pipelineInfo,
								  nullptr,
								  &m_computePipeline) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create graphics pipeline");
	}
}

void PgsComputePipeline::createShaderModule(const std::vector<char> &code, VkShaderModule *shaderModule)
{
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());

	if (vkCreateShaderModule(m_pgsDevice.device(), &createInfo, nullptr, shaderModule) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create shader module");
	}
}

void PgsComputePipeline::bind(VkCommandBuffer commandBuffer)
{
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, m_computePipeline);
}

void PgsComputePipeline::compute(VkCommandBuffer commandBuffer)
{
	vkCmdDispatch(commandBuffer, PgsModel::PARTICLE_COUNT / 256, 1, 1);
}

} // namespace pgs
