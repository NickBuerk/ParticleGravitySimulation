#pragma once

#include "pgs_device.hpp"
#include "pgs_swap_chain.hpp"
#include "pgs_window.hpp"

// std
#include <cassert>
#include <memory>
#include <vector>

namespace pgs
{
class PgsRenderer
{
  public:
	PgsRenderer(PgsWindow &window, PgsDevice &device);
	~PgsRenderer();

	PgsRenderer(const PgsRenderer &) = delete;
	PgsRenderer &operator=(const PgsRenderer &) = delete;

	VkRenderPass getSwapChainRenderPass() const
	{
		return m_pgsSwapChain->getRenderPass();
	}
	float getAspectRatio() const
	{
		return m_pgsSwapChain->extentAspectRatio();
	}
	bool isFrameInProgress() const
	{
		return m_isFrameStarted;
	}

	VkCommandBuffer getCurrentCommandBuffer() const
	{
		assert(m_isFrameStarted && "Cannot get command buffer when frame not in progress");
		return m_commandBuffers[m_currentFrameIndex];
	}

	int getFrameIndex() const
	{
		assert(m_isFrameStarted && "Cannot get frame index when frame not in progress");
		return m_currentFrameIndex;
	}

	VkCommandBuffer beginFrame();
	void endFrame();
	void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
	void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

  private:
	void createCommandBuffers();
	void freeCommandBuffers();
	void recreateSwapChain();

	PgsWindow &m_pgsWindow;
	PgsDevice &m_pgsDevice;
	std::unique_ptr<PgsSwapChain> m_pgsSwapChain;
	std::vector<VkCommandBuffer> m_commandBuffers;

	uint32_t m_currentImageIndex;
	int m_currentFrameIndex{0};
	bool m_isFrameStarted{false};
};
} // namespace pgs
