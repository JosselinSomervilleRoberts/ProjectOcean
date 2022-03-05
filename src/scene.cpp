#include "scene.hpp"


using namespace cgp;


void scene_structure::display()
{
	// Basics common elements
	// ***************************************** //
	timer.update();
	environment.light = environment.camera.position();
	if (gui.display_frame)
		draw(global_frame, environment);

	// Simulation of the ocean
	// ***************************************** //
	compute_vertex_position(ocean, original_position, waves, timer.t);
	
	if (gui.display_noise) {
		compte_Perlin_noise(ocean, original_position, perlin_noise);
	}

	// ocean display
	// ***************************************** //
	
	// Prepare to display the updated ocean
	ocean.update_normal();        // compute the new normals
	ocean_drawable.update(ocean); // update the positions on the GPU

	// Display the ocean
	draw(ocean_drawable, environment);
	if(gui.display_wireframe)
		draw_wireframe(ocean_drawable, environment);
}


// Compute a new ocean in its initial position (can be called multiple times)
void scene_structure::initialize_ocean(int N_sample)
{
	ocean.initialize(N_sample);
	ocean_drawable.initialize(N_sample);
	ocean_drawable.drawable.shading.color = cgp::vec3(0.3f, 0.3f, 1.0f);

	// save the original position for wave function computation
	original_position = ocean.position;
	add_waves(waves, 10, cgp::vec2(0.f, 1.f));

	//ocean_drawable.drawable.texture = ocean_texture;
}


void scene_structure::initialize()
{
	global_frame.initialize(mesh_primitive_frame(), "Frame");
	//environment.camera.look_at({0.0f, 1.0f, 1.5f}, { 0,0,0.5f }, { 0,0,1.0f });
	environment.camera.look_at({0.0f, 70, 20}, { 0,0,0 }, { 0,0,1.0f});

	ocean_texture = opengl_load_texture_image("assets/ocean.jpg");
	initialize_ocean(gui.N_sample_edge);

}

void scene_structure::display_gui()
{
	bool reset = false;

	ImGui::Text("Display");
	ImGui::Checkbox("Wireframe", &gui.display_wireframe);
	ImGui::Checkbox("Perlin Noise", &gui.display_noise);

	ImGui::Spacing(); ImGui::Spacing();

/*
	ImGui::Text("Simulation parameters");
	ImGui::SliderFloat("Time step", &parameters.dt, 0.01f, 0.02f);
	ImGui::SliderFloat("Stiffness", &parameters.K, 1.0f, 80.0f, "%.3f", 2.0f);
	ImGui::SliderFloat("Wind magnitude", &parameters.wind.magnitude, 0, 60, "%.3f", 2.0f);
	ImGui::SliderFloat("Damping", &parameters.mu, 1.0f, 100.0f);
	ImGui::SliderFloat("Mass", &parameters.mass_total, 0.2f, 5.0f, "%.3f", 2.0f);

	ImGui::Spacing(); ImGui::Spacing();
*/	
	ImGui::SliderInt("Edge Number", &gui.N_sample_edge, 50, 150);
	ImGui::SliderFloat("persistency", &perlin_noise.persistency, 0.0f, 1.0f);
	ImGui::SliderFloat("frequency_gain", &perlin_noise.frequency_gain, 0, 5.0f);
	ImGui::SliderInt("octave", &perlin_noise.octave, 0, 10);

	ImGui::Spacing(); ImGui::Spacing();
	reset |= ImGui::Button("Restart");
	if (reset) {
		initialize_ocean(gui.N_sample_edge);
		simulation_running = true;
	}
	
}


