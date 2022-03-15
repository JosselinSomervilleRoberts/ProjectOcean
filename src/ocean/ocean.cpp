#include "ocean.hpp"
#include "../scene.hpp"

using namespace cgp;
#define PI 3.1415f


Ocean::Ocean() {}

/* Simulation by sum of random waves */

void Ocean::initialize(int N_samples_edge_arg)
{
    assert_cgp(N_samples_edge_arg > 3, "N_samples_edge=" + str(N_samples_edge_arg) + " should be > 3");

    position.clear();
    normal.clear();

    position.resize(N_samples_edge_arg, N_samples_edge_arg);
    normal.resize(N_samples_edge_arg, N_samples_edge_arg);

    float const z0 = 1.0f;
    float const dz = 10;
    float const size = 100;
    mesh const ocean_mesh = mesh_primitive_grid({ -size,-size,z0 }, { size,-size,z0 }, { size,size,z0 }, { -size,size,z0 }, N_samples_edge_arg, N_samples_edge_arg).fill_empty_field();
    position = grid_2D<vec3>::from_buffer(ocean_mesh.position, N_samples_edge_arg, N_samples_edge_arg);
    normal = grid_2D<vec3>::from_buffer(ocean_mesh.normal, N_samples_edge_arg, N_samples_edge_arg);
    triangle_connectivity = ocean_mesh.connectivity;

    // Fond
    float EPSILON = 0.001f;
    mesh const fond_mesh = mesh_primitive_grid({ -size- EPSILON,-size- EPSILON,z0-dz }, { size+EPSILON,-size- EPSILON,z0-dz }, { size+ EPSILON,size+ EPSILON,z0-dz }, { -size- EPSILON,size+ EPSILON,z0-dz }, N_samples_edge_arg, N_samples_edge_arg).fill_empty_field();
    fond.clear();
    fond.initialize(fond_mesh, "fond");
    fond.texture = opengl_load_texture_image("assets/ocean.jpg");
    fond.shading.use_texture = true;
    fond.shading.phong.specular = 0.3f;
    fond.shading.phong.diffuse = 0.5f;
    fond.shading.phong.ambient = 0.1f;
    fond.shader = opengl_load_shader("shaders/fond/vert.glsl", "shaders/fond/frag.glsl");

    // Drawable
    drawable.clear();
    drawable.initialize(ocean_mesh, "ocean");
    drawable.texture = opengl_load_texture_image("assets/ocean.jpg");
    drawable.shading.color = cgp::vec3(0.3f, 0.3f, 1.0f);

    // Noise
    perlin.used = true;
    perlin.amplitude = 1.8f;
    perlin.octave = 5;
    perlin.persistency = 0.4f;
    perlin.frequency = 1.0f;
    perlin.frequency_gain = 2.2f;
    perlin.dilatation_space = 0.07f;
    perlin.dilatation_time = 0.5f;

    // Waves
    wave_exponant = 7.0f;
    N_waves_desired = 20;

    // Wind
    wind.magnitude = 5.0f;
    wind.direction = cgp::vec2(1.f, 0.f);
}

void Ocean::update_normal()
{
    normal_per_vertex(position.data, triangle_connectivity, normal.data);
}

void Ocean::update()
{    
    update_waves();
}

void Ocean::add_random_waves(size_t N) {
    for (int i = 0; i < N; i++) {
        wave_parameters wave;
        wave.angular_velocity = 2 * PI * ((double)rand() / (RAND_MAX));
        float angle = PI * (-0.5f + ((double)rand() / (RAND_MAX)));
        cgp::vec2 dir;
        dir.x = std::cos(angle) * wind.direction.x + std::sin(angle) * wind.direction.y;
        dir.y = std::cos(angle) * wind.direction.y - std::sin(angle) * wind.direction.x;
        wave.amplitude = ((double)rand() / (RAND_MAX));
        wave.K = 0.3f * wave.angular_velocity * wave.angular_velocity /9.8f * dir;
        wave.dir = normalize(wave.K);

        waves.push_back(wave);
    }
}

void Ocean::update_waves() {
    if (waves.size() == N_waves_desired) return;
    else if (waves.size() < N_waves_desired) {
        add_random_waves(N_waves_desired - waves.size());
    }
    else {
        waves.resize(N_waves_desired);
    }
}

/* Functions to generate GPU */

void Ocean::draw(Scene& scene, float t)
{
    drawable.shading.phong.specular = scene.getSpecular();
    drawable.shading.phong.diffuse  = scene.getDiffuse();
    drawable.shading.phong.ambient  = scene.getAmbient();
    drawable.shading.use_texture    = scene.getUseTexture();
    drawable.shading.phong.specular_exponent = scene.getSpecularExponant();

    fond.shading.phong.specular = scene.getSpecular();
    fond.shading.phong.diffuse = scene.getDiffuse();
    fond.shading.phong.ambient = scene.getAmbient();
    fond.shading.use_texture = scene.getUseTexture();
    fond.shading.phong.specular_exponent = scene.getSpecularExponant();

    cgp::scene_environment_basic const& environment = scene.environment;
    scene.send_lights_to_GPU(fond.shader);
    cgp::draw(fond, environment);

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
    scene.send_lights_to_GPU(drawable.shader);
    opengl_uniform(drawable.shader, "env_mapping_coeff", env_mapping_coeff);

	// Set texture
	glActiveTexture(GL_TEXTURE0); opengl_check;
	glBindTexture(GL_TEXTURE_2D, drawable.texture); opengl_check;
	opengl_uniform(drawable.shader, "image_texture", 0);  opengl_check;

    // Set texture as a cubemap (different from the 2D texture using in the "standard" draw call) as a second texture
    glActiveTexture(GL_TEXTURE1); opengl_check;
    glBindTexture(GL_TEXTURE_CUBE_MAP, environment_map_texture); opengl_check;
    opengl_uniform(drawable.shader, "environment_image_texture", 1, false);  opengl_check;
    // Note: The value 'expected' is set to false so that this draw() call remains valid even if the shader doesn't expect an environment_image_texture parameter

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
        float amplitude = waves[i].amplitude * wind.magnitude * std::pow(std::fabs(cgp::dot(normalize(waves[i].K), wind.direction)), wave_exponant) / (float)(std::sqrt(waves.size()));
        opengl_uniform(drawable.shader, "waves[" + str(i) + "].amplitude", amplitude);
        opengl_uniform(drawable.shader, "waves[" + str(i) + "].angular_velocity", waves[i].angular_velocity);
        opengl_uniform(drawable.shader, std::string("waves[" + str(i) + "].K"), waves[i].K);
        opengl_uniform(drawable.shader, std::string("waves[" + str(i) + "].dir"), waves[i].dir);
    }

    opengl_uniform(drawable.shader, "ecume_threshold", ecume_threshold);
    opengl_uniform(drawable.shader, "ecume_exponent", ecume_exponent);
}

void Ocean::send_noise_to_GPU() {
    opengl_uniform(drawable.shader, "noise.used", perlin.used);
    opengl_uniform(drawable.shader, "noise.amplitude", perlin.amplitude);
    opengl_uniform(drawable.shader, "noise.dilatation_space", perlin.dilatation_space);
    opengl_uniform(drawable.shader, "noise.dilatation_time", perlin.dilatation_time);
    opengl_uniform(drawable.shader, "noise.frequency", perlin.frequency);
    opengl_uniform(drawable.shader, "noise.frequency_gain", perlin.frequency_gain);
    opengl_uniform(drawable.shader, "noise.persistency", perlin.persistency);
    opengl_uniform(drawable.shader, "noise.octave", perlin.octave);
}


cgp::vec3 Ocean::getVertexPos(cgp::vec3 position, float time) {
    return compute_wave_pos(position, time, waves.size(), waves, perlin);
}