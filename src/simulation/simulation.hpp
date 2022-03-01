#pragma once

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
    cgp::buffer<float> amplitude = {0.5f, 0.3f}; 
    cgp::buffer<float> frequency = {2*3.14f, 3.14f}; 
    cgp::buffer<cgp::vec2> direction = {{1,1}, {0,1}};
};

void compute_vertex_position(ocean_structure & ocean, cgp::grid_2D<cgp::vec3> original_position, waves_parameters waves, float t);
