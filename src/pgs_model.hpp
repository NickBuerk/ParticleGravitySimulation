#pragma once

#include "pgs_buffer.hpp"
#include "pgs_device.hpp"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

// std
#include <memory>
#include <vector>

namespace pgs
{
class PgsModel
{
  public:
	struct Particle
	{
		glm::vec2 position{};
		glm::vec2 velocity{};

		static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
		static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

		bool operator==(const Particle &other) const
		{
			return position == other.position && velocity == other.velocity;
		}
	};

	static constexpr uint32_t PARTICLE_COUNT = 256 * 256;

	PgsModel(PgsDevice &device, const std::vector<Particle> &particles);
	~PgsModel();

	PgsModel(const PgsModel &) = delete;
	PgsModel &operator=(const PgsModel &) = delete;

	static std::unique_ptr<PgsModel> createModel(PgsDevice &device);
	std::unique_ptr<PgsBuffer> &getVertexBuffer()
	{
		return vertexBuffer;
	}

	void bind(VkCommandBuffer commandBuffer);
	void draw(VkCommandBuffer commandBuffer);

  private:
	void createVertexBuffers(const std::vector<Particle> &particles);

	PgsDevice &pgsDevice;

	std::vector<Particle> vertices{};
	std::unique_ptr<PgsBuffer> vertexBuffer;
	uint32_t vertexCount;

	bool hasIndexBuffer = false;
	std::unique_ptr<PgsBuffer> indexBuffer;
	uint32_t indexCount;
};
} // namespace pgs
