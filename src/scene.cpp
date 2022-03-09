#include "scene.hpp"


using namespace cgp;


void scene_structure::display()
{
	timer.update();
	environment.light = environment.camera.position();
	if (gui.display_frame)
		draw(global_frame, environment);

	// Display the ocean
	draw(skybox, environment);

	ocean.update();
	ship.update(ocean, timer.t);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	ship.draw(environment);
	ocean.draw(environment, timer.t);
	glDisable(GL_BLEND);
}


// Compute a new ocean in its initial position (can be called multiple times)
void scene_structure::initialize_ocean(int N_sample)
{
	ocean.initialize(N_sample);
	ocean.drawable.shading.color = cgp::vec3(0.3f, 0.3f, 1.0f);
	original_position = ocean.position;
}


void scene_structure::initialize()
{
	global_frame.initialize(mesh_primitive_frame(), "Frame");
	//environment.camera.look_at({0.0f, 1.0f, 1.5f}, { 0,0,0.5f }, { 0,0,1.0f });
	environment.camera.look_at({0.0f, 70, 20}, { 0,0,0 }, { 0,0,1.0f});

	ocean_texture = opengl_load_texture_image("assets/ocean.jpg");
	initialize_ocean(gui.N_sample_edge);
	skybox.initialize("assets/skybox/");         // indicate a path where to load the 6 texture images

	ship.initialize();
}

void scene_structure::display_gui()
{
	bool reset = false;

	ImGui::Text("MESH SETTINGS");
	ImGui::SliderInt("Edge Number", &gui.N_sample_edge, 100, 1000);
	ImGui::Checkbox("Wireframe", &ocean.show_wireframe);

	ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
	ImGui::Text("WAVES SETTINGS");
	ImGui::SliderInt("Wave Number", &ocean.N_waves_desired, 0, 100);
	ImGui::SliderFloat("Exponant", &ocean.wave_exponant, 0.1f, 8.0f);

	ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
	ImGui::Text("WIND SETTINGS");
	ImGui::SliderFloat("Magnitude", &ocean.wind.magnitude, 0.0f, 5.0f);
	
	ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
	ImGui::Text("PERLIN NOISE");
	ImGui::Checkbox("Active", &ocean.perlin.used);
	ImGui::SliderFloat("Amplitude", &ocean.perlin.amplitude, 0.0f, 5.0f);
	ImGui::SliderFloat("Persistency", &ocean.perlin.persistency, 0.0f, 1.5f);
	ImGui::SliderFloat("Frequency", &ocean.perlin.frequency, 0.1f, 5.0f);
	ImGui::SliderFloat("Frequency Gain", &ocean.perlin.frequency_gain, 0, 5.0f);
	ImGui::SliderInt("Octave", &ocean.perlin.octave, 0, 10);
	ImGui::SliderFloat("Dilatation Space", &ocean.perlin.dilatation_space, 0.01f, 0.2f);
	ImGui::SliderFloat("Dilatation Time", &ocean.perlin.dilatation_time, 0.1f, 2.0f);

	ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
	ImGui::Text("RENDERING");
	ImGui::SliderFloat("Ambient", &ocean.drawable.shading.phong.ambient, 0.0f, 1.0f);
	ImGui::SliderFloat("Diffuse", &ocean.drawable.shading.phong.diffuse, 0.0f, 1.0f);
	ImGui::SliderFloat("Specular", &ocean.drawable.shading.phong.specular, 0.0f, 1.0f);
	ImGui::SliderFloat("Specular Exponent", &ocean.drawable.shading.phong.specular_exponent, 0.0f, 100.0f);
	ImGui::SliderFloat("Light Intensity", &ocean.light_intensity, 0.0f, 2.0f);
	ImGui::Checkbox("Use Texture", &ocean.drawable.shading.use_texture);

	ImGui::Spacing(); ImGui::Spacing();
	reset |= ImGui::Button("Restart");
	if (reset) {
		initialize_ocean(gui.N_sample_edge);
		simulation_running = true;
	}
	
}