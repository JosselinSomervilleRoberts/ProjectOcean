#include "scene.hpp"


using namespace cgp;

Scene::Scene() {}

void Scene::display()
{
	// Update the elements
	update();

	// Draw the elements
	environment.light = environment.camera.position();
	draw(skybox, environment);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	ship.draw(environment);
	ocean.draw(*this, timer.t);
	glDisable(GL_BLEND);
	if (gui.display_frame) draw(global_frame, environment);
}

void Scene::update() {
	if (simulation_running) timer.update();
	ocean.update();
	ship.update(ocean, timer.t);
}



void Scene::initialize()
{
	// Intialize environment
	global_frame.initialize(mesh_primitive_frame(), "Frame");
	//environment.camera.look_at({0.0f, 1.0f, 1.5f}, { 0,0,0.5f }, { 0,0,1.0f });
	environment.camera.look_at({0.0f, 70, 20}, { 0,0,0 }, { 0,0,1.0f});

	// Initialize elements to draw
	ocean.initialize(gui.N_sample_edge);
	skybox.initialize("assets/skybox/");
	ship.initialize();

	// Lights
	lights.push_back({ cgp::vec3(0, 0, -1), 2.0f, cgp::vec3(1.0f, 1.0f, 1.0f) });
	//lights.push_back({ normalize(vec3(2,4,-1)), 3.0f, vec3(1.0f,0.0f,0.0f) });
	//lights.push_back({ normalize(vec3(-2,4,-1)), 3.0f, vec3(0.0f,1.0f,0.0f) });
	light_intensity = 1.0f;
}

void Scene::display_gui()
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
	ImGui::Text("SHIP SETTINGS");
	ImGui::SliderInt("Triangle Number", &ship.N_triangles_desired, 3, 50);
	ImGui::SliderFloat("Mass", &ship.m, 1.0f, 300.0f);
	ImGui::SliderFloat("Friction", &ship.K, 0.0f, 100.0f);

	ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
	ImGui::Text("RENDERING");
	ImGui::SliderFloat("Ecume Threshold", &ocean.ecume_threshold, 0.0f, 1.0f);
	ImGui::SliderFloat("Ecume Exponent", &ocean.ecume_exponent, 0.0f, 1.0f);
	ImGui::SliderFloat("Ambient", &ocean.drawable.shading.phong.ambient, 0.0f, 1.0f);
	ImGui::SliderFloat("Diffuse", &ocean.drawable.shading.phong.diffuse, 0.0f, 1.0f);
	ImGui::SliderFloat("Specular", &ocean.drawable.shading.phong.specular, 0.0f, 1.0f);
	ImGui::SliderFloat("Specular Exponent", &ocean.drawable.shading.phong.specular_exponent, 0.0f, 100.0f);
	ImGui::SliderFloat("Light Intensity", &this->light_intensity, 0.0f, 2.0f);
	ImGui::Checkbox("Use Texture", &ocean.drawable.shading.use_texture);

	ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
	ImGui::Text("SIMULATION SETTINGS");
	ImGui::Checkbox("Display Frame", &gui.display_frame);
	ImGui::Checkbox("Running", &simulation_running);
	reset |= ImGui::Button("Restart");
	if (reset) {
		ocean.initialize(gui.N_sample_edge);
		simulation_running = true;
	}
	
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