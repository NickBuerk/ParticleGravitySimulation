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

	PgsWindow pgsWindow{WIDTH, HEIGHT, "Particle Gravity Simulation"};
	PgsDevice pgsDevice{pgsWindow};
	PgsRenderer pgsRenderer{pgsWindow, pgsDevice};

	// note: order of declarations matters
	std::unique_ptr<PgsDescriptorPool> globalPool{};
};
} // namespace pgs
