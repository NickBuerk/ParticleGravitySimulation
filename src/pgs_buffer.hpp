#pragma once

#include "pgs_device.hpp"

namespace pgs
{

class PgsBuffer
{
  public:
	PgsBuffer(PgsDevice &device,
			  VkDeviceSize instanceSize,
			  uint32_t instanceCount,
			  VkBufferUsageFlags usageFlags,
			  VkMemoryPropertyFlags memoryPropertyFlags,
			  VkDeviceSize minOffsetAlignment = 1);
	~PgsBuffer();

	PgsBuffer(const PgsBuffer &) = delete;
	PgsBuffer &operator=(const PgsBuffer &) = delete;

	VkResult map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
	void unmap();

	void writeToBuffer(void *data, VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
	VkResult flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
	VkDescriptorBufferInfo descriptorInfo(VkDeviceSize size = VK_WHOLE_SIZE,
										  VkDeviceSize offset = 0);
	VkResult invalidate(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

	void writeToIndex(void *data, int index);
	VkResult flushIndex(int index);
	VkDescriptorBufferInfo descriptorInfoForIndex(int index);
	VkResult invalidateIndex(int index);

	VkBuffer getBuffer() const
	{
		return m_buffer;
	}
	void *getMappedMemory() const
	{
		return m_mapped;
	}
	uint32_t getInstanceCount() const
	{
		return m_instanceCount;
	}
	VkDeviceSize getInstanceSize() const
	{
		return m_instanceSize;
	}
	VkDeviceSize getAlignmentSize() const
	{
		return m_instanceSize;
	}
	VkBufferUsageFlags getUsageFlags() const
	{
		return m_usageFlags;
	}
	VkMemoryPropertyFlags getMemoryPropertyFlags() const
	{
		return m_memoryPropertyFlags;
	}
	VkDeviceSize getBufferSize() const
	{
		return m_bufferSize;
	}

  private:
	static VkDeviceSize getAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment);

	PgsDevice &m_pgsDevice;
	void *m_mapped = nullptr;
	VkBuffer m_buffer = VK_NULL_HANDLE;
	VkDeviceMemory m_memory = VK_NULL_HANDLE;

	VkDeviceSize m_bufferSize;
	uint32_t m_instanceCount;
	VkDeviceSize m_instanceSize;
	VkDeviceSize m_alignmentSize;
	VkBufferUsageFlags m_usageFlags;
	VkMemoryPropertyFlags m_memoryPropertyFlags;
};

} // namespace pgs
