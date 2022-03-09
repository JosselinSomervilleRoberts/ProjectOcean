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
struct scene_structure {
	
	// ****************************** //
	// Elements and shapes of the scene
	// ****************************** //

	cgp::mesh_drawable global_frame;          // The standard global frame
	cgp::scene_environment_basic environment; // Standard environment controler
	gui_parameters gui;                       // Standard GUI element storage
	
	// ****************************** //
	// Elements and shapes of the scene
	// ****************************** //
	cgp::timer_basic timer;

	// ocean related structures
	
	cgp::grid_2D<cgp::vec3> original_position; // Extra storage of the original position of the mesh


	Ship ship;
	Ocean ocean;                     // The values of the position, velocity, forces, etc, stored as a 2D grid
	cgp::skybox_drawable skybox;
	// Helper variables
	bool simulation_running = true;   // Boolean indicating if the simulation should be computed
	GLuint ocean_texture;             // Storage of the texture ID used for the ocean

	// ****************************** //
	// Functions
	// ****************************** //

	void initialize();  // Standard initialization to be called before the animation loop
	void display();     // The frame display to be called within the animation loop
	void display_gui(); // The display of the GUI, also called within the animation loop

	void initialize_ocean(int N_sample); // Recompute the ocean from scratch
};





