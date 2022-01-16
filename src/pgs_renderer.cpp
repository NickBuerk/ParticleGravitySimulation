#include "pgs_renderer.hpp"
#include "pgs_window.hpp"

// std
#include <array>
#include <cassert>
#include <stdexcept>

namespace pgs
{

pgsRenderer::pgsRenderer(PgsWindow &window, PgsDevice &device)
	: m_pgsWindow{window}, pgsDevice{device}
{
	recreateSwapChain();
	createCommandBuffers();
}

pgsRenderer::~pgsRenderer()
{
	freeCommandBuffers();
}

void pgsRenderer::recreateSwapChain()
{
	auto extent = m_pgsWindow.getExtent();
	while (extent.width == 0 || extent.height == 0)
	{
		extent = m_pgsWindow.getExtent();
		glfwWaitEvents();
	}
	vkDeviceWaitIdle(pgsDevice.device());

	if (pgsSwapChain == nullptr)
	{
		pgsSwapChain = std::make_unique<PgsSwapChain>(pgsDevice, extent);
	}
	else
	{
		std::shared_ptr<PgsSwapChain> oldSwapChain = std::move(pgsSwapChain);
		pgsSwapChain = std::make_unique<PgsSwapChain>(pgsDevice, extent, oldSwapChain);

		if (!oldSwapChain->compareSwapFormats(*pgsSwapChain.get()))
		{
			throw std::runtime_error("Swap chain image(or depth) format has changed!");
		}
	}
}

void pgsRenderer::createCommandBuffers()
{
	commandBuffers.resize(PgsSwapChain::MAX_FRAMES_IN_FLIGHT);

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = pgsDevice.getCommandPool();
	allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

	if (vkAllocateCommandBuffers(pgsDevice.device(), &allocInfo, commandBuffers.data()) !=
		VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate command buffers!");
	}
}

void pgsRenderer::freeCommandBuffers()
{
	vkFreeCommandBuffers(pgsDevice.device(),
						 pgsDevice.getCommandPool(),
						 static_cast<uint32_t>(commandBuffers.size()),
						 commandBuffers.data());
	commandBuffers.clear();
}

VkCommandBuffer pgsRenderer::beginFrame()
{
	assert(!isFrameStarted && "Can't call beginFrame while already in progress");

	auto result = pgsSwapChain->acquireNextImage(&currentImageIndex);
	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		recreateSwapChain();
		return nullptr;
	}

	if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
	{
		throw std::runtime_error("failed to acquire swap chain image!");
	}

	isFrameStarted = true;

	auto commandBuffer = getCurrentCommandBuffer();
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to begin recording command buffer!");
	}
	return commandBuffer;
}

void pgsRenderer::endFrame()
{
	assert(isFrameStarted && "Can't call endFrame while frame is not in progress");
	auto commandBuffer = getCurrentCommandBuffer();
	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to record command buffer!");
	}

	auto result = pgsSwapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
		m_pgsWindow.wasWindowResized())
	{
		m_pgsWindow.resetWindowResizedFlag();
		recreateSwapChain();
	}
	else if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to present swap chain image!");
	}

	isFrameStarted = false;
	currentFrameIndex = (currentFrameIndex + 1) % PgsSwapChain::MAX_FRAMES_IN_FLIGHT;
}

void pgsRenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer)
{
	assert(isFrameStarted && "Can't call beginSwapChainRenderPass if frame is not in progress");
	assert(commandBuffer == getCurrentCommandBuffer() &&
		   "Can't begin render pass on command buffer from a different frame");

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = pgsSwapChain->getRenderPass();
	renderPassInfo.framebuffer = pgsSwapChain->getFrameBuffer(currentImageIndex);

	renderPassInfo.renderArea.offset = {0, 0};
	renderPassInfo.renderArea.extent = pgsSwapChain->getSwapChainExtent();

	std::array<VkClearValue, 2> clearValues{};
	clearValues[0].color = {0.01f, 0.01f, 0.01f, 1.0f};
	clearValues[1].depthStencil = {1.0f, 0};
	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(pgsSwapChain->getSwapChainExtent().width);
	viewport.height = static_cast<float>(pgsSwapChain->getSwapChainExtent().height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	VkRect2D scissor{{0, 0}, pgsSwapChain->getSwapChainExtent()};
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}

void pgsRenderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer)
{
	assert(isFrameStarted && "Can't call endSwapChainRenderPass if frame is not in progress");
	assert(commandBuffer == getCurrentCommandBuffer() &&
		   "Can't end render pass on command buffer from a different frame");
	vkCmdEndRenderPass(commandBuffer);
}

} // namespace pgs
