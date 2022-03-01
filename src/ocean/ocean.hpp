#pragma once

#include "cgp/cgp.hpp"

// Stores the buffers representing the ocean vertices
struct ocean_structure
{    
    // Buffers are stored as 2D grid that can be accessed as grid(ku,kv)
    cgp::grid_2D<cgp::vec3> position;
    cgp::grid_2D<cgp::vec3> normal;

    // Also stores the triangle connectivity used to update the normals
    cgp::buffer<cgp::uint3> triangle_connectivity;

    
    void initialize(int N_samples_edge);  // Initialize a square flat ocean
    void update_normal();       // Call this function every time the ocean is updated before its draw
    int N_samples_edge() const; // Number of vertex along one dimension of the grid
};


// Helper structure and functions to draw a ocean
// ********************************************** //
struct ocean_structure_drawable{
    cgp::mesh_drawable drawable;

    void initialize(int N_sample_edge);
    void update(ocean_structure const& ocean);
};


template <typename ENVIRONMENT>
void draw(ocean_structure_drawable const& drawable, ENVIRONMENT const& environment)
{
    draw(drawable.drawable, environment);
}
template <typename ENVIRONMENT>
void draw_wireframe(ocean_structure_drawable const& drawable, ENVIRONMENT const& environment)
{
    draw_wireframe(drawable.drawable, environment);
}