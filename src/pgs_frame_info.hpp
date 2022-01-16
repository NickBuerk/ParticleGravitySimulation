#pragma once

#include "pgs_model.hpp"

// lib
#include <vulkan/vulkan.h>

#include <memory>

namespace pgs
{
struct FrameInfo
{
	int frameIndex;
	float frameTime;
	VkCommandBuffer commandBuffer;
	std::shared_ptr<pgsModel> model;
	VkDescriptorSet globalDescriptorSet;
};
} // namespace pgs
