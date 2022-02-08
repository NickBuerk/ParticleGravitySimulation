#pragma once

#include "pgs_descriptors.hpp"
#include "pgs_device.hpp"
#include "pgs_renderer.hpp"
#include "pgs_window.hpp"

// std
#include <memory>
#include <vector>

namespace pgs
{
class GravSimApp
{
  public:
	static constexpr int WIDTH = 2560;
	static constexpr int HEIGHT = 1440;

	GravSimApp();
	~GravSimApp();

	GravSimApp(const GravSimApp &) = delete;
	GravSimApp &operator=(const GravSimApp &) = delete;

	void run();

  private:
	void loadGameObjects();

	PgsWindow m_pgsWindow{WIDTH, HEIGHT, "Particle Gravity Simulation"};
	PgsDevice m_pgsDevice{m_pgsWindow};
	PgsRenderer m_pgsRenderer{m_pgsWindow, m_pgsDevice};

	// note: order of declarations matters
	std::unique_ptr<PgsDescriptorPool> globalPool{};
};
} // namespace pgs
