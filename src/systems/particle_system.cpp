#include "particle_system.hpp"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <stdexcept>
#include <cassert>

namespace pgs
{
    ParticleSystem::ParticleSystem(PgsDevice &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) : m_Device{device}
    {
        createGraphicsPipelineLayout();
        createGraphicsPipeline(renderPass);
        //createComputePipelineLayout(/*globalSetLayout*/);
        //createComputePipeline();
    }

    ParticleSystem::~ParticleSystem()
    {
        vkDestroyPipelineLayout(m_Device.device(), m_graphicsPipelineLayout, nullptr);
        //vkDestroyPipelineLayout(m_Device.device(), m_computePipelineLayout, nullptr);
    }

    void ParticleSystem::createGraphicsPipelineLayout()
    {
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;
        if (vkCreatePipelineLayout(m_Device.device(), &pipelineLayoutInfo, nullptr, &m_graphicsPipelineLayout) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to create graphics pipeline layout!");
        }
    }

    void ParticleSystem::createGraphicsPipeline(VkRenderPass renderPass) {
        assert(m_graphicsPipelineLayout != nullptr && "Cannot create graphics pipeline before graphics pipeline layout");

        PipelineConfigInfo pipelineConfig{};
        PgsPipeline::defaultPipelineConfigInfo(pipelineConfig);
        pipelineConfig.renderPass = renderPass;
        pipelineConfig.pipelineLayout = m_graphicsPipelineLayout;
        m_graphicsPipeline = std::make_unique<PgsPipeline>(
            m_Device,
            "shaders/particle.vert.spv",
            "shaders/particle.frag.spv",
            pipelineConfig);
    }

    // void ParticleSystem::createComputePipelineLayout(/*VkDescriptorSetLayout globalSetLayout*/)
    // {
    //     // std::vector<VkDescriptorSetLayout> descriptorSetLayouts{globalSetLayout};

    //     VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    //     pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    //     pipelineLayoutInfo.setLayoutCount = 0; //static_cast<uint32_t>(descriptorSetLayouts.size());
    //     pipelineLayoutInfo.pSetLayouts = nullptr; //descriptorSetLayouts.data();
    //     pipelineLayoutInfo.pushConstantRangeCount = 0;
    //     pipelineLayoutInfo.pPushConstantRanges = nullptr;
    //     if (vkCreatePipelineLayout(m_Device.device(), &pipelineLayoutInfo, nullptr, &m_computePipelineLayout) !=
    //         VK_SUCCESS) {
    //         throw std::runtime_error("failed to create compute pipeline layout!");
    //     }
    // }

    // void ParticleSystem::createComputePipeline() {
    //     assert(m_computePipelineLayout != nullptr && "Cannot create compute pipeline before compute pipeline layout");

    //     ComputePipelineConfigInfo pipelineConfig{};
    //     pgsComputePipeline::particlePipelineConfigInfo(pipelineConfig);
    //     pipelineConfig.pipelineLayout = m_computePipelineLayout;
    //     m_computePipeline = std::make_unique<pgsComputePipeline>(
    //         m_Device,
    //         "shaders/particle.comp.spv",
    //         pipelineConfig);
    // }

    void ParticleSystem::renderParticles(FrameInfo& frameInfo) {

        // Wait for previous vertex shader invocations to complete.
        //VkMemoryBarrier;

        // bind compute pipeline
        //m_computePipeline->bind(frameInfo.commandBuffer);

        // bind the descriptor set

        // dispatch compute job
        //vkCmdDispatch(frameInfo.commandBuffer, );

        /*vkCmdBindDescriptorSets(
            frameInfo.commandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            m_pipelineLayout,
            0,
            1,
            &frameInfo.globalDescriptorSet,
            0,
            nullptr);*/

        // render the model
        m_graphicsPipeline->bind(frameInfo.commandBuffer);
        frameInfo.model->bind(frameInfo.commandBuffer);
        frameInfo.model->draw(frameInfo.commandBuffer);
    }

} //namespace pgs