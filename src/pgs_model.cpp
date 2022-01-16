#include "pgs_model.hpp"

#include "pgs_utils.hpp"

// std
#include <cassert>
#include <cstring>
#include <random>

namespace pgs
{

PgsModel::PgsModel(PgsDevice &device, const std::vector<Particle> &particles) : pgsDevice{device}
{
	createVertexBuffers(particles);
}

PgsModel::~PgsModel()
{
}

std::unique_ptr<PgsModel> PgsModel::createModel(PgsDevice &device)
{
	std::default_random_engine rndEngine((unsigned)time(nullptr));
	std::uniform_real_distribution<float> rndDistribution(-1.0f, 1.0f);

	std::vector<Particle> particles(PARTICLE_COUNT);
	for (auto &particle : particles)
	{
		particle.position = glm::vec2(rndDistribution(rndEngine), rndDistribution(rndEngine));
		particle.velocity = glm::vec2{0.f, 0.f};
	}

	return std::make_unique<PgsModel>(device, particles);
}

void PgsModel::createVertexBuffers(const std::vector<Particle> &particles)
{
	vertexCount = static_cast<uint32_t>(particles.size());
	VkDeviceSize bufferSize = sizeof(particles[0]) * vertexCount;
	uint32_t vertexSize = sizeof(particles[0]);

	PgsBuffer stagingBuffer{
		pgsDevice,
		vertexSize,
		vertexCount,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
	};

	stagingBuffer.map();
	stagingBuffer.writeToBuffer((void *)particles.data());

	vertexBuffer = std::make_unique<PgsBuffer>(pgsDevice,
											   vertexSize,
											   vertexCount,
											   VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
												   VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
												   VK_BUFFER_USAGE_TRANSFER_DST_BIT,
											   VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	pgsDevice.copyBuffer(stagingBuffer.getBuffer(), vertexBuffer->getBuffer(), bufferSize);
}

void PgsModel::bind(VkCommandBuffer commandBuffer)
{
	VkBuffer buffers[] = {vertexBuffer->getBuffer()};
	VkDeviceSize offsets[] = {0};
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
}

void PgsModel::draw(VkCommandBuffer commandBuffer)
{
	vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
}

std::vector<VkVertexInputBindingDescription> PgsModel::Particle::getBindingDescriptions()
{
	std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
	bindingDescriptions[0].binding = 0;
	bindingDescriptions[0].stride = sizeof(Particle);
	bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	return bindingDescriptions;
}

std::vector<VkVertexInputAttributeDescription> PgsModel::Particle::getAttributeDescriptions()
{
	std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

	attributeDescriptions.push_back({0, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Particle, position)});
	attributeDescriptions.push_back({1, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Particle, velocity)});

	return attributeDescriptions;
}
} // namespace pgs
