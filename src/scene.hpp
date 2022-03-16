#pragma once

#include "cgp/cgp.hpp"
#include "ocean/ocean.hpp"
#include "ship/ship.hpp"


// The element of the GUI that are not already stored in other structures
struct gui_parameters {
	bool display_frame     = false;
	bool display_wireframe = false;
	int N_sample_edge = 500;  // number of samples of the ocean (the total number of vertices is N_sample_edge^2)
};

// The structure of the custom scene
class Scene {
	
public:
	Scene();

	void initialize();  // Standard initialization to be called before the animation loop
	void display();     // The frame display to be called within the animation loop
	void update();
	void display_gui(); // The display of the GUI, also called within the animation loop

	void send_lights_to_GPU(GLuint shader);

	cgp::scene_environment_basic environment; // Standard environment controler

	float getSpecular() { return specular; };
	float getDiffuse()  { return diffuse;  };
	float getAmbient()  { return ambient;  };
	float getSpecularExponant() { return specular_exponent; };
	bool getUseTexture() { return use_texture; };

	void changeScene(int skybox_id_copy);

private:
	// Drawing elements helpers
	cgp::mesh_drawable global_frame;          // The standard global frame
	gui_parameters gui;                       // Standard GUI element storage

	// Elements to draw
	Ship ship;
	Ocean ocean;              
	cgp::skybox_drawable skybox_night;
	cgp::skybox_drawable skybox_sunset;
	bool display_ship = true;
	int skybox_id = 2;
	
	// For simulation
	cgp::timer_basic timer;
	bool simulation_running = true;

	// Lights
	std::vector<LightSourceDir> lights;
	float light_intensity;
	float specular = 0.4f;
	float diffuse = 0.7f;
	float ambient = 0.4f;
	float specular_exponent = 64.0f;
	bool use_texture = true;
};





