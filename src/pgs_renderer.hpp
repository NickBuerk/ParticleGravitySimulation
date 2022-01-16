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
class pgsRenderer
{
  public:
	pgsRenderer(PgsWindow &window, PgsDevice &device);
	~pgsRenderer();

	pgsRenderer(const pgsRenderer &) = delete;
	pgsRenderer &operator=(const pgsRenderer &) = delete;

	VkRenderPass getSwapChainRenderPass() const
	{
		return pgsSwapChain->getRenderPass();
	}
	float getAspectRatio() const
	{
		return pgsSwapChain->extentAspectRatio();
	}
	bool isFrameInProgress() const
	{
		return isFrameStarted;
	}

	VkCommandBuffer getCurrentCommandBuffer() const
	{
		assert(isFrameStarted && "Cannot get command buffer when frame not in progress");
		return commandBuffers[currentFrameIndex];
	}

	int getFrameIndex() const
	{
		assert(isFrameStarted && "Cannot get frame index when frame not in progress");
		return currentFrameIndex;
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
	PgsDevice &pgsDevice;
	std::unique_ptr<PgsSwapChain> pgsSwapChain;
	std::vector<VkCommandBuffer> commandBuffers;

	uint32_t currentImageIndex;
	int currentFrameIndex{0};
	bool isFrameStarted{false};
};
} // namespace pgs
