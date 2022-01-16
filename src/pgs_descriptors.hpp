#pragma once

#include "pgs_device.hpp"

// std
#include <memory>
#include <unordered_map>
#include <vector>

namespace pgs
{

class pgsDescriptorSetLayout
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
		std::unique_ptr<pgsDescriptorSetLayout> build() const;

	  private:
		PgsDevice &m_pgsDevice;
		std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
	};

	pgsDescriptorSetLayout(PgsDevice &pgsDevice,
						   std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
	~pgsDescriptorSetLayout();
	pgsDescriptorSetLayout(const pgsDescriptorSetLayout &) = delete;
	pgsDescriptorSetLayout &operator=(const pgsDescriptorSetLayout &) = delete;

	VkDescriptorSetLayout getDescriptorSetLayout() const
	{
		return descriptorSetLayout;
	}

  private:
	PgsDevice &m_pgsDevice;
	VkDescriptorSetLayout descriptorSetLayout;
	std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

	friend class pgsDescriptorWriter;
};

class pgsDescriptorPool
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
		std::unique_ptr<pgsDescriptorPool> build() const;

	  private:
		PgsDevice &m_pgsDevice;
		std::vector<VkDescriptorPoolSize> poolSizes{};
		uint32_t maxSets = 1000;
		VkDescriptorPoolCreateFlags poolFlags = 0;
	};

	pgsDescriptorPool(PgsDevice &pgsDevice,
					  uint32_t maxSets,
					  VkDescriptorPoolCreateFlags poolFlags,
					  const std::vector<VkDescriptorPoolSize> &poolSizes);
	~pgsDescriptorPool();
	pgsDescriptorPool(const pgsDescriptorPool &) = delete;
	pgsDescriptorPool &operator=(const pgsDescriptorPool &) = delete;

	bool allocateDescriptor(const VkDescriptorSetLayout descriptorSetLayout,
							VkDescriptorSet &descriptor) const;

	void freeDescriptors(std::vector<VkDescriptorSet> &descriptors) const;

	void resetPool();

  private:
	PgsDevice &m_pgsDevice;
	VkDescriptorPool descriptorPool;

	friend class pgsDescriptorWriter;
};

class pgsDescriptorWriter
{
  public:
	pgsDescriptorWriter(pgsDescriptorSetLayout &setLayout, pgsDescriptorPool &pool);

	pgsDescriptorWriter &writeBuffer(uint32_t binding, VkDescriptorBufferInfo *bufferInfo);
	pgsDescriptorWriter &writeImage(uint32_t binding, VkDescriptorImageInfo *imageInfo);

	bool build(VkDescriptorSet &set);
	void overwrite(VkDescriptorSet &set);

  private:
	pgsDescriptorSetLayout &setLayout;
	pgsDescriptorPool &pool;
	std::vector<VkWriteDescriptorSet> writes;
};

} // namespace pgs
