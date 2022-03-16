#include "scene.hpp"


using namespace cgp;

Scene::Scene() {}

void Scene::display()
{
	// Update the elements
	update();

	// Draw the skybox
	environment.light = environment.camera.position();
	if (skybox_id == 1) draw(skybox_night, environment);
	else if (skybox_id == 2) draw(skybox_sunset, environment);

	// Draw the elements
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	ship.draw(*this, display_ship);
	ocean.draw(*this, timer.t);
	glDisable(GL_BLEND);
	if (gui.display_frame) draw(global_frame, environment);
}

void Scene::update() {
	if (simulation_running) timer.update();
	ocean.update();
	if(display_ship) ship.update(ocean, timer.t);
}



void Scene::initialize()
{
	// Intialize environment
	global_frame.initialize(mesh_primitive_frame(), "Frame");
	//environment.camera.look_at({0.0f, 1.0f, 1.5f}, { 0,0,0.5f }, { 0,0,1.0f });
	environment.camera.look_at({0.0f, 70, 20}, { 0,0,0 }, { 0,0,1.0f});

	// Initialize elements to draw
	ocean.initialize(gui.N_sample_edge);
	skybox_night.initialize("assets/skybox_night/");
	skybox_sunset.initialize("assets/skybox/");
	ship.initialize();

	// Initialize the scene
	changeScene(2);
}

void Scene::display_gui()
{
	bool reset = false;

	ImGui::Text("MESH SETTINGS"); ImGui::SameLine();
	ImGui::Checkbox("Wireframe", &ocean.show_wireframe);
	ImGui::SliderInt("Edge Number", &gui.N_sample_edge, 100, 1000);

	ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
	ImGui::Text("WAVES SETTINGS");
	ImGui::SliderInt("Wave Number", &ocean.N_waves_desired, 0, 100);
	ImGui::SliderFloat("Exponant", &ocean.wave_exponant, 0.1f, 8.0f);

	ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
	ImGui::Text("WIND SETTINGS");
	ImGui::SliderFloat("Magnitude", &ocean.wind.magnitude, 0.0f, 8.0f);
	
	ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
	ImGui::Text("PERLIN NOISE"); ImGui::SameLine();
	ImGui::Checkbox("Active", &ocean.perlin.used);
	ImGui::SliderFloat("Amplitude", &ocean.perlin.amplitude, 0.0f, 1.0f);
	ImGui::SliderFloat("Persistency", &ocean.perlin.persistency, 0.0f, 1.5f);
	ImGui::SliderFloat("Frequency", &ocean.perlin.frequency, 0.1f, 5.0f);
	ImGui::SliderFloat("Frequency Gain", &ocean.perlin.frequency_gain, 0, 5.0f);
	ImGui::SliderInt("Octave", &ocean.perlin.octave, 0, 10);
	ImGui::SliderFloat("Dilatation Space", &ocean.perlin.dilatation_space, 0.01f, 0.2f);
	ImGui::SliderFloat("Dilatation Time", &ocean.perlin.dilatation_time, 0.1f, 2.0f);

	ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
	ImGui::Text("SHIP SETTINGS"); ImGui::SameLine();
	ImGui::Checkbox("Display", &display_ship);
	ImGui::SliderInt("Triangle Number", &ship.N_triangles_desired, 3, 50);
	ImGui::SliderInt("Noise octaves", &ship.nb_octaves, 0, 10);
	ImGui::SliderFloat("Mass", &ship.m, 1.0f, 100.0f);
	ImGui::SliderFloat("Friction", &ship.K, 0.0f, 50.0f);

	ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
	ImGui::Text("SEAFOAM PARAMETERS"); ImGui::SameLine();
	ImGui::Checkbox("Display", &ocean.foam.display); ImGui::SameLine();
	ImGui::Checkbox("Only", &ocean.foam.only);
	ImGui::SliderInt("Noise Octaves", &ocean.foam.nbOctaves, 0, 10);
	ImGui::SliderFloat("ha", &ocean.foam.ha, 0.0f, 1.0f);
	ImGui::SliderFloat("alpha_g", &ocean.foam.alpha_g, 1.0f, 20.0f);
	ImGui::SliderFloat("h0", &ocean.foam.h0, 0.0f, 1.0f);
	ImGui::SliderFloat("hepsilon", &ocean.foam.hepsilon, 0.0f, 1.0f);
	ImGui::SliderFloat("Height threshold", &ocean.foam.compute_threshold, 0.0f, 2.0f);
	ImGui::SliderFloat("Render Threshold", &ocean.foam.render_threshold, 0.0f, 1.0f);
	ImGui::SliderFloat("Render exponent", &ocean.foam.exponent, 0.0f, 4.0f);
	
	ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
	ImGui::Text("RENDERING"); ImGui::SameLine();
	ImGui::Checkbox("Use Texture", &use_texture);
	ImGui::SliderFloat("Environment Mapping", &ocean.env_mapping_coeff, 0.0f, 1.0f);
	ImGui::SliderFloat("Ambient", &ambient, 0.0f, 1.0f);
	ImGui::SliderFloat("Diffuse", &diffuse, 0.0f, 1.0f);
	ImGui::SliderFloat("Specular", &specular, 0.0f, 1.0f);
	ImGui::SliderFloat("Specular Exponent", &specular_exponent, 0.0f, 100.0f);
	ImGui::SliderFloat("Light Intensity", &this->light_intensity, 0.0f, 2.0f);

	ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
	ImGui::Text("SIMULATION SETTINGS"); ImGui::SameLine();
	ImGui::Checkbox("Running", &simulation_running); ImGui::SameLine();
	reset |= ImGui::Button("Restart");
	if (reset) {
		ocean.initialize(gui.N_sample_edge);
		simulation_running = true;
	}
	ImGui::Text("Scene:"); ImGui::SameLine();
	int skybox_id_copy = skybox_id;
	int* ptr_shape_display = reinterpret_cast<int*>(&skybox_id_copy);
	ImGui::RadioButton("None", ptr_shape_display, 0); ImGui::SameLine();
	ImGui::RadioButton("Night", ptr_shape_display, 1); ImGui::SameLine();
	ImGui::RadioButton("Sunset", ptr_shape_display, 2);
	if (skybox_id_copy != skybox_id) changeScene(skybox_id_copy);
}

void Scene::send_lights_to_GPU(GLuint shader) {
	int N_lights = lights.size();
	opengl_uniform(shader, "nb_lightsourcesDir", N_lights);

	for (int i = 0; i < N_lights; i++) {
		opengl_uniform(shader, "lightsourcesDir[" + str(i) + "].direction", lights[i].direction);
		opengl_uniform(shader, "lightsourcesDir[" + str(i) + "].intensity", lights[i].intensity * light_intensity);
		opengl_uniform(shader, "lightsourcesDir[" + str(i) + "].color", lights[i].color);
	}
}

void Scene::changeScene(int skybox_id_copy) {
	skybox_id = skybox_id_copy;
	light_intensity = 1.0f;

	if (skybox_id == 0) {
		// Environment
		ocean.use_environment_map = false;
		ocean.env_mapping_coeff = 0;

		// Render
		specular = 0.4f;
		diffuse = 0.7f;
		ambient = 0.4f;

		//Lights
		lights.clear();
		lights.push_back({ cgp::normalize(cgp::vec3(0, 0, -1)), 2.5f, cgp::vec3(1.0f, 1.0f, 1.0f) });
	}
	else if (skybox_id == 1) {
		// Environment
		ocean.use_environment_map = true;
		ocean.environment_map_texture = skybox_night.texture;
		ocean.env_mapping_coeff = 0.9f;

		// Render
		specular = 0.65f;
		diffuse = 0.75f;
		ambient = 0.1f;

		//Lights
		lights.clear();
		lights.push_back({ cgp::normalize(cgp::vec3(-0.6, +0.7, -2.5f)), 1.15f, cgp::vec3(1.0f, 1.0f, 1.0f) });
	}
	else if (skybox_id == 2) {
		// Environment
		ocean.use_environment_map = true;
		ocean.environment_map_texture = skybox_sunset.texture;
		ocean.env_mapping_coeff = 0.6f;

		// Render
		specular = 0.4f;
		diffuse = 0.7f;
		ambient = 0.4f;

		//Lights
		lights.clear();
		lights.push_back({ cgp::normalize(cgp::vec3(0.6, -0.7, -0.5)), 2.0f, cgp::vec3(1.0f, 1.0f, 0.5f) });
		lights.push_back({ cgp::normalize(cgp::vec3(0.6, -0.3, -0.8)), 2.0f, cgp::vec3(1.0f, 0.6f, 0.1f) });
		lights.push_back({ cgp::normalize(cgp::vec3(0.3, -0.6, -0.8)), 2.0f, cgp::vec3(1.0f, 0.4f, 0.05f) });
	}
}