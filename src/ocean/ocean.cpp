#include "ocean.hpp"

using namespace cgp;


void ocean_structure::initialize(int N_samples_edge_arg)
{
    assert_cgp(N_samples_edge_arg > 3, "N_samples_edge=" + str(N_samples_edge_arg) + " should be > 3");

    position.clear();
    normal.clear();

    position.resize(N_samples_edge_arg, N_samples_edge_arg);
    normal.resize(N_samples_edge_arg, N_samples_edge_arg);
    
    float const z0 = 1.0f;
    mesh const ocean_mesh = mesh_primitive_grid({ -50,-50,z0 }, { 50,-50,z0 }, { 50,50,z0 }, { -50,50,z0 }, N_samples_edge_arg, N_samples_edge_arg).fill_empty_field();
    position = grid_2D<vec3>::from_buffer(ocean_mesh.position, N_samples_edge_arg, N_samples_edge_arg);
    normal = grid_2D<vec3>::from_buffer(ocean_mesh.normal, N_samples_edge_arg, N_samples_edge_arg);
    triangle_connectivity = ocean_mesh.connectivity;
}

void ocean_structure::update_normal()
{
    normal_per_vertex(position.data, triangle_connectivity, normal.data);
}

int ocean_structure::N_samples_edge() const{
    return position.dimension.x;
}


void ocean_structure_drawable::initialize(int N_samples_edge)
{
    mesh const ocean_mesh = mesh_primitive_grid({-50,-50,0},{50,-50,0},{50,50,0},{-50,50,0}, N_samples_edge, N_samples_edge);
    drawable.clear();
    drawable.initialize(ocean_mesh, "ocean");
    drawable.shading.phong.specular = 0.0f;
}


void ocean_structure_drawable::update(ocean_structure const& ocean)
{    
    drawable.update_position(ocean.position.data);
    drawable.update_normal(ocean.normal.data);
}
