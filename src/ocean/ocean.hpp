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

/*

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
void draw(ocean_structure_drawable const& drawable_arg, ENVIRONMENT const& environment, float t, waves_parameters waves)
{
    auto scene = environment;
    auto drawable = drawable_arg.drawable;

	if (drawable.number_triangles == 0) return;

	// Setup shader
	assert_cgp(drawable.shader != 0, "Try to draw mesh_drawable without shader [name:" + drawable.name + "]");
	assert_cgp(drawable.texture != 0, "Try to draw mesh_drawable without texture [name:" + drawable.name + "]");
	glUseProgram(drawable.shader); opengl_check;

	// Send uniforms for this shader
	opengl_uniform(drawable.shader, environment);
	opengl_uniform(drawable.shader, drawable.shading);
	opengl_uniform(drawable.shader, "model", drawable.model_matrix());

	// Time
	opengl_uniform(drawable.shader, "t", t);

	// Set texture
	glActiveTexture(GL_TEXTURE0); opengl_check;
	glBindTexture(GL_TEXTURE_2D, drawable.texture); opengl_check;
	opengl_uniform(drawable.shader, "image_texture", 0);  opengl_check;

	// Call draw function
	assert_cgp(drawable.number_triangles > 0, "Try to draw mesh_drawable with 0 triangles [name:" + drawable.name + "]"); opengl_check;
	glBindVertexArray(drawable.vao);   opengl_check;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drawable.vbo.at("index")); opengl_check;
	glDrawElements(GL_TRIANGLES, GLsizei(drawable.number_triangles * 3), GL_UNSIGNED_INT, nullptr); opengl_check;

	// Clean buffers
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}
template <typename ENVIRONMENT>
void draw_wireframe(ocean_structure_drawable const& drawable, ENVIRONMENT const& environment)
{
    draw_wireframe(drawable.drawable, environment);
}*/