#pragma once

#include "pgs_device.hpp"

// std
#include <memory>
#include <unordered_map>
#include <vector>

namespace pgs
{

class PgsDescriptorSetLayout
{
  public:
	class Builder
	{
	  public:
		Builder(PgsDevice &pgsDevice) : m_pgsDevice{pgsDevice}
		{
		}

		Builder &addBinding(uint32_t binding,
							VkDescriptorType descriptorType,
							VkShaderStageFlags stageFlags,
							uint32_t count = 1);
		std::unique_ptr<PgsDescriptorSetLayout> build() const;

	  private:
		PgsDevice &m_pgsDevice;
		std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
	};

	PgsDescriptorSetLayout(PgsDevice &pgsDevice,
						   std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
	~PgsDescriptorSetLayout();
	PgsDescriptorSetLayout(const PgsDescriptorSetLayout &) = delete;
	PgsDescriptorSetLayout &operator=(const PgsDescriptorSetLayout &) = delete;

	VkDescriptorSetLayout getDescriptorSetLayout() const
	{
		return descriptorSetLayout;
	}

  private:
	PgsDevice &m_pgsDevice;
	VkDescriptorSetLayout descriptorSetLayout;
	std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

	friend class PgsDescriptorWriter;
};

class PgsDescriptorPool
{
  public:
	class Builder
	{
	  public:
		Builder(PgsDevice &pgsDevice) : m_pgsDevice{pgsDevice}
		{
		}

		Builder &addPoolSize(VkDescriptorType descriptorType, uint32_t count);
		Builder &setPoolFlags(VkDescriptorPoolCreateFlags flags);
		Builder &setMaxSets(uint32_t count);
		std::unique_ptr<PgsDescriptorPool> build() const;

	  private:
		PgsDevice &m_pgsDevice;
		std::vector<VkDescriptorPoolSize> poolSizes{};
		uint32_t maxSets = 1000;
		VkDescriptorPoolCreateFlags poolFlags = 0;
	};

	PgsDescriptorPool(PgsDevice &pgsDevice,
					  uint32_t maxSets,
					  VkDescriptorPoolCreateFlags poolFlags,
					  const std::vector<VkDescriptorPoolSize> &poolSizes);
	~PgsDescriptorPool();
	PgsDescriptorPool(const PgsDescriptorPool &) = delete;
	PgsDescriptorPool &operator=(const PgsDescriptorPool &) = delete;

	bool allocateDescriptor(const VkDescriptorSetLayout descriptorSetLayout,
							VkDescriptorSet &descriptor) const;

	void freeDescriptors(std::vector<VkDescriptorSet> &descriptors) const;

	void resetPool();

  private:
	PgsDevice &m_pgsDevice;
	VkDescriptorPool descriptorPool;

	friend class PgsDescriptorWriter;
};

class PgsDescriptorWriter
{
  public:
	PgsDescriptorWriter(PgsDescriptorSetLayout &setLayout, PgsDescriptorPool &pool);

	PgsDescriptorWriter &writeBuffer(uint32_t binding, VkDescriptorBufferInfo *bufferInfo);
	PgsDescriptorWriter &writeImage(uint32_t binding, VkDescriptorImageInfo *imageInfo);

	bool build(VkDescriptorSet &set);
	void overwrite(VkDescriptorSet &set);

  private:
	PgsDescriptorSetLayout &setLayout;
	PgsDescriptorPool &pool;
	std::vector<VkWriteDescriptorSet> writes;
};

} // namespace pgs
