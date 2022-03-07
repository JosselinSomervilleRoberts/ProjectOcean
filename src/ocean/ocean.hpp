#pragma once

#include "cgp/cgp.hpp"
#include "simulation/simulation.hpp"


struct wave_parameters {
	float amplitude;
	float frequency;
	cgp::vec2 direction;
};

struct perlin_noise_parameters {
	bool used;
	float amplitude;
	int octave;
	float persistency;
	float frequency;
	float frequency_gain;
	float dilatation_space;
	float dilatation_time;
};

struct wind_parameters {
	float magnitude = 0.0f;
	cgp::vec2 direction = { 0,-1};
};

struct LightSourceDir {
	cgp::vec3 direction;
	float intensity;
	cgp::vec3 color;
};

class Ocean {
public:

	Ocean();
	void initialize(int N_samples_edge);
	void draw(cgp::scene_environment_basic const& environment, float t);
	void update_normal();
	void update();

	void add_random_waves(size_t N);
	void update_waves();

	// GPU functions
	void send_waves_to_GPU();
	void send_noise_to_GPU();
	void send_lights_to_GPU();


	// To generate
	std::vector<wave_parameters> waves;
	perlin_noise_parameters perlin;
	wind_parameters wind;
	float wave_exponant;
	int N_waves_desired;

	// Geometry
	cgp::grid_2D<cgp::vec3> position;
	cgp::grid_2D<cgp::vec3> normal; // Normally not used
	cgp::buffer<cgp::uint3> triangle_connectivity;

	// Drawable
	cgp::mesh_drawable drawable;
	bool show_wireframe = false;
	std::vector<LightSourceDir> lights;
	float light_intensity;
};