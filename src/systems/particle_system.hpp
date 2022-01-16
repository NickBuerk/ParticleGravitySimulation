#pragma once

#include "../pgs_device.hpp"
#include "../pgs_frame_info.hpp"
#include "pgs_pipeline.hpp"
//#include "pgs_computePipeline.hpp"
#include "pgs_buffer.hpp"

// std
#include <memory>
#include <vector>

namespace pgs {
class ParticleSystem {

 public:
  ParticleSystem(PgsDevice &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
  ~ParticleSystem();

  ParticleSystem(const ParticleSystem &) = delete;
  ParticleSystem &operator=(const ParticleSystem &) = delete;

  void renderParticles(FrameInfo &frameInfo);

 private:
  void createGraphicsPipelineLayout();
  void createGraphicsPipeline(VkRenderPass renderPass);
  void createComputePipelineLayout(VkDescriptorSetLayout globalSetLayout);
  void createComputePipeline();

  PgsDevice &m_Device;

  std::unique_ptr<pgsPipeline> m_graphicsPipeline;
  VkPipelineLayout m_graphicsPipelineLayout;
  //std::unique_ptr<pgsComputePipeline> m_computePipeline;
  //VkPipelineLayout m_computePipelineLayout;
};
}  // namespace pgs
