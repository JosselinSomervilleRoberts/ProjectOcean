#include "ocean.hpp"

using namespace cgp;
#define PI 3.1415f


Ocean::Ocean() {}


void Ocean::initialize(int N_samples_edge_arg)
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

    // Drawable
    drawable.clear();
    drawable.initialize(ocean_mesh, "ocean");
    drawable.shading.phong.specular = 0.0f;

    // Noise
    perlin.amplitude = 2.5f;
    perlin.octave = 2;
    perlin.persistency = 0.2;
    perlin.frequency = 1.0f;
    perlin.frequency_gain = 2;
    perlin.dilatation_space = 0.1f;
    perlin.dilatation_time = 1.0f;
}

void Ocean::update_normal()
{
    normal_per_vertex(position.data, triangle_connectivity, normal.data);
}



void Ocean::update()
{    
    drawable.update_position(position.data);
    drawable.update_normal(normal.data);
}


void Ocean::draw(cgp::scene_environment_basic const& environment, float t)
{
	auto scene = environment;
	if (drawable.number_triangles == 0) return;

	// Setup shader
	assert_cgp(drawable.shader != 0, "Try to draw mesh_drawable without shader [name:" + drawable.name + "]");
	assert_cgp(drawable.texture != 0, "Try to draw mesh_drawable without texture [name:" + drawable.name + "]");
	glUseProgram(drawable.shader); opengl_check;

	// Send uniforms for this shader
	opengl_uniform(drawable.shader, environment);
	opengl_uniform(drawable.shader, drawable.shading);
	opengl_uniform(drawable.shader, "model", drawable.model_matrix());

	// Data
	opengl_uniform(drawable.shader, "t", t);
    send_waves_to_GPU();
    send_noise_to_GPU();

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

    if(show_wireframe) draw_wireframe(drawable, environment);
}



void Ocean::add_random_waves(size_t N, cgp::vec2 global_dir) {
    waves.resize(N);
   
    for (int i = 0; i < N; i++) {
        waves[i].frequency = PI * (1 + 3 * ((double)rand() / (RAND_MAX)));
        float angle = PI * (0.5f + ((double)rand() / (RAND_MAX)));
        cgp::vec2 dir;
        dir.x = std::cos(angle) * global_dir.x + std::sin(angle) * global_dir.y;
        dir.y = std::cos(angle) * global_dir.y - std::sin(angle) * global_dir.x;
        waves[i].direction = dir;
        waves[i].amplitude = 3.0f * ((double)rand() / (RAND_MAX)) / (float)(std::sqrt(N)) * std::pow(std::fabs(cgp::dot(dir, global_dir)), 4.0f);
    }
}

void opengl_uniform(GLuint shader, std::string name, vec2 const& value)
{
    assert_cgp(shader != 0, "Try to send uniform " + name + " to unspecified shader");
    GLint const location = glGetUniformLocation(shader, name.c_str()); opengl_check;
    glUniform2f(location, value.x, value.y); opengl_check;
}


void Ocean::send_waves_to_GPU() {
    int N_waves = waves.size();
    opengl_uniform(drawable.shader, "N_waves", N_waves);
    
    for (int i = 0; i < N_waves; i++) {
        opengl_uniform(drawable.shader, "waves[" + str(i) + "].amplitude", waves[i].amplitude);
        opengl_uniform(drawable.shader, "waves[" + str(i) + "].frequency", waves[i].frequency);
        opengl_uniform(drawable.shader, std::string("waves[" + str(i) + "].direction"), waves[i].direction);
        //opengl_uniform(drawable.shader, "waves[" + str(i) + "].directionY", waves[i].direction.y);
    }
}

void Ocean::send_noise_to_GPU() {
    opengl_uniform(drawable.shader, "noise.amplitude", perlin.amplitude);
    opengl_uniform(drawable.shader, "noise.dilatation_space", perlin.dilatation_space);
    opengl_uniform(drawable.shader, "noise.dilatation_time", perlin.dilatation_time);
    opengl_uniform(drawable.shader, "noise.frequency", perlin.frequency);
    opengl_uniform(drawable.shader, "noise.frequency_gain", perlin.frequency_gain);
    opengl_uniform(drawable.shader, "noise.persistency", perlin.persistency);
    opengl_uniform(drawable.shader, "noise.octave", perlin.octave);
}