#pragma once

#include "pgs_device.hpp"

// std
#include <string>
#include <vector>

namespace pgs
{

class PgsComputePipeline
{
  public:
	PgsComputePipeline(PgsDevice &device,
				const std::string &compFilepath,
				const VkPipelineLayout &pipelineLayout);
	~PgsComputePipeline();

	PgsComputePipeline(const PgsComputePipeline &) = delete;
	PgsComputePipeline &operator=(const PgsComputePipeline &) = delete;

	void bind(VkCommandBuffer commandBuffer);
	void compute(VkCommandBuffer commandBuffer);

  private:
	void createComputePipeline(const std::string &compFilepath,
								const VkPipelineLayout &pipelineLayout);

	void createShaderModule(const std::vector<char> &code, VkShaderModule *shaderModule);

	PgsDevice &m_pgsDevice;
	VkPipeline m_computePipeline;
	VkShaderModule m_compShaderModule;
};
} // namespace pgs
