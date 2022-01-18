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

std::vector<pgs::PgsModel::Particle> particleDist1()
{
	std::default_random_engine rndEngine((unsigned)time(nullptr));
	std::uniform_real_distribution<float> rndDistribution(-0.6f, 0.6f);

	std::vector<pgs::PgsModel::Particle> particles(pgs::PgsModel::PARTICLE_COUNT);
	for (auto &particle : particles)
	{
		particle.position = glm::vec2(rndDistribution(rndEngine), rndDistribution(rndEngine));
		float distanceToCenter = glm::dot(particle.position, particle.position);
		glm::vec2 directionPerpendicularToCenterLine =
			glm::vec2(-particle.position.y, particle.position.x);
		particle.velocity = distanceToCenter * directionPerpendicularToCenterLine;
	}

	return particles;
}

std::vector<pgs::PgsModel::Particle> particleDist2()
{
	std::vector<pgs::PgsModel::Particle> particles(pgs::PgsModel::PARTICLE_COUNT);
	glm::vec2 center1(-0.3f, -0.3f);
	glm::vec2 center2(0.3f, 0.3f);
	float radius1 = 0.2f;
	float radius2 = 0.2f;
	const float velocityDamping = 0.5f;

	std::default_random_engine rndEngine((unsigned)time(nullptr));
	std::uniform_real_distribution<float> rndDist1(center1.x - radius1, center1.y + radius1);
	std::uniform_real_distribution<float> rndDist2(center2.x - radius2, center2.y + radius2);

	// First half of particles in dist1, second half in dist2
	for (size_t i = 0; i < particles.size() / 2; i++)
	{
		pgs::PgsModel::Particle &particle1 = particles[i];
		pgs::PgsModel::Particle &particle2 = particles[2 * i];

		// Particle 1
		particle1.position = glm::vec2(rndDist1(rndEngine), rndDist1(rndEngine));
		float distanceToCenter1 =
			glm::dot(center1 - particle1.position, center1 - particle1.position);
		glm::vec2 directionPerpendicularToCenterLine1 =
			glm::vec2(-particle1.position.y, particle1.position.x);
		particle1.velocity =
			distanceToCenter1 * directionPerpendicularToCenterLine1 * velocityDamping;

		// Particle 2
		particle2.position = glm::vec2(rndDist2(rndEngine), rndDist2(rndEngine));
		float distanceToCenter2 =
			glm::dot(center2 - particle2.position, center2 - particle2.position);
		glm::vec2 directionPerpendicularToCenterLine2 =
			glm::vec2(-particle2.position.y, particle2.position.x);
		particle2.velocity =
			distanceToCenter2 * directionPerpendicularToCenterLine2 * velocityDamping;
	}

	return particles;
}

std::unique_ptr<PgsModel> PgsModel::createModel(PgsDevice &device)
{
	std::vector<Particle> particles = particleDist1();

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
	attributeDescriptions.push_back(
		{2, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Particle, color)});

	return attributeDescriptions;
}
} // namespace pgs
