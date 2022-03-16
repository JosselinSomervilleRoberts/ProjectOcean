#pragma once

#include "cgp/cgp.hpp"
#include "../wave.hpp"
#include "structures.hpp"

class Scene;

class Ocean {
public:

	Ocean();
	void initialize(int N_samples_edge);
	void draw(Scene& environment, float t);
	void update_normal();
	void update();

	void add_random_waves(size_t N);
	void update_waves();

	// GPU functions
	void send_waves_to_GPU();
	void send_noise_to_GPU();
	void send_seafoam_to_GPU();
	void send_environment_to_GPU();

	// Shader functions translated to C++
	cgp::vec3 getVertexPos(cgp::vec3 position, float time, int noiseOctave);


	// To generate
	std::vector<wave_parameters> waves;
	perlin_noise_parameters perlin;
	wind_parameters wind;
	float wave_exponant;
	int N_waves_desired;

	// Geometry used for FFT
	cgp::grid_2D<cgp::vec3> position;
	cgp::grid_2D<cgp::vec3> normal;
	cgp::buffer<cgp::uint3> triangle_connectivity;

	// Drawable
	cgp::mesh_drawable drawable;
	cgp::mesh_drawable fond;
	bool show_wireframe = false;
	SeaFoam foam;

	// Environment map
	GLuint environment_map_texture = 0;
	bool use_environment_map = true;
	float env_mapping_coeff = 0.6f;
};