#include "gravSimApp.hpp"

#include "pgs_buffer.hpp"
#include "systems/particle_system.hpp"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <array>
#include <cassert>
#include <chrono>
#include <iostream>
#include <stdexcept>

namespace pgs
{

struct GlobalUbo
{
	alignas(16) float frameTime;
	uint32_t particleCount;
};

GravSimApp::GravSimApp()
{
	globalPool =
		PgsDescriptorPool::Builder(m_pgsDevice)
			.setMaxSets(PgsSwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, PgsSwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, PgsSwapChain::MAX_FRAMES_IN_FLIGHT)
			.build();
}

GravSimApp::~GravSimApp()
{
}

void GravSimApp::run()
{
	std::vector<std::unique_ptr<PgsBuffer>> uboBuffers(PgsSwapChain::MAX_FRAMES_IN_FLIGHT);
	for (int i = 0; i < uboBuffers.size(); i++)
	{
		uboBuffers[i] = std::make_unique<PgsBuffer>(m_pgsDevice,
													sizeof(GlobalUbo),
													1,
													VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
													VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
		uboBuffers[i]->map();
	}

	auto globalSetLayout =
		PgsDescriptorSetLayout::Builder(m_pgsDevice)
			.addBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
			.addBinding(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
			.build();

	ParticleSystem particleSystem{m_pgsDevice,
								  m_pgsRenderer.getSwapChainRenderPass(),
								  globalSetLayout->getDescriptorSetLayout()};

	std::shared_ptr<PgsModel> pgsModel = PgsModel::createModel(m_pgsDevice);

	std::vector<VkDescriptorSet> globalDescriptorSets(PgsSwapChain::MAX_FRAMES_IN_FLIGHT);
	for (int i = 0; i < globalDescriptorSets.size(); i++)
	{
		auto bufferInfo = uboBuffers[i]->descriptorInfo();
		auto storageInfo = pgsModel->getVertexBuffer()->descriptorInfo();
		auto result = PgsDescriptorWriter(*globalSetLayout, *globalPool)
						  .writeBuffer(0, &storageInfo)
						  .writeBuffer(1, &bufferInfo)
						  .build(globalDescriptorSets[i]);
		assert(result && "Failed to build descriptor writer!");
	}

	auto currentTime = std::chrono::high_resolution_clock::now();
	while (!m_pgsWindow.shouldClose())
	{
		glfwPollEvents();

		auto newTime = std::chrono::high_resolution_clock::now();
		float frameTime =
			std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime)
				.count();
		currentTime = newTime;
		// std::cout << "Frame time: " << frameTime << "\n";

		if (auto commandBuffer = m_pgsRenderer.beginFrame())
		{
			int frameIndex = m_pgsRenderer.getFrameIndex();
			FrameInfo frameInfo{frameIndex,
								frameTime,
								commandBuffer,
								pgsModel,
								globalDescriptorSets[frameIndex]};

			// update
			GlobalUbo ubo{};
			ubo.frameTime = frameTime;
			ubo.particleCount = PgsModel::PARTICLE_COUNT;
			uboBuffers[frameIndex]->writeToBuffer(&ubo);
			uboBuffers[frameIndex]->flush();

			// render
			particleSystem.computeParticles(frameInfo);
			m_pgsRenderer.beginSwapChainRenderPass(commandBuffer);
			particleSystem.renderParticles(frameInfo);
			m_pgsRenderer.endSwapChainRenderPass(commandBuffer);
			m_pgsRenderer.endFrame();
		}
	}

	vkDeviceWaitIdle(m_pgsDevice.device());
}

} // namespace pgs
