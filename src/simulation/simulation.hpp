#pragma once

/*
#include "cgp/cgp.hpp"
#include "../ocean/ocean.hpp"

struct simulation_parameters{
    float dt = 0.08f;        // time step for the numerical integration

    //  Wind magnitude and direction
    struct {
        float magnitude = 0.0f;
        cgp::vec3 direction = { 0,-1,0 };
    } wind;
  
};

struct waves_parameters{
    cgp::buffer<float> amplitude; // = { 0.5f, 0.3f };
    cgp::buffer<float> frequency; // = { 2 * 3.14f, 3.14f };
    cgp::buffer<cgp::vec2> direction; // = { {1,1}, {0,1} };
};

struct perlin_noise_parameters{
	float persistency = 0.7f;
	float frequency_gain = 3.0f;
	int octave = 7;
	float height = 0;
};

void add_waves(waves_parameters& waves, size_t N, cgp::vec2 global_dir);
void compute_vertex_position(ocean_structure & ocean, cgp::grid_2D<cgp::vec3> original_position, waves_parameters waves, float t);
void compte_Perlin_noise(ocean_structure &ocean, cgp::grid_2D<cgp::vec3> original_position, perlin_noise_parameters const& perlin);
*/