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

private:
	// Drawing elements helpers
	cgp::mesh_drawable global_frame;          // The standard global frame
	gui_parameters gui;                       // Standard GUI element storage

	// Elements to draw
	Ship ship;
	Ocean ocean;              
	cgp::skybox_drawable skybox;
	
	// For simulation
	cgp::timer_basic timer;
	bool simulation_running = false;

	// Lights
	std::vector<LightSourceDir> lights;
	float light_intensity;
};





