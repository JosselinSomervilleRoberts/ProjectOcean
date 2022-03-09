#pragma once

#include "cgp/cgp.hpp"
#include "ocean/ocean.hpp"

class Scene;

class Ship {
public:

	Ship();
	void initialize();
	void draw(Scene& scene);

	void computeRefPoints();

	void update(Ocean& ocean, float time);
	void computeRotation(float dt);
	void computeTranslation(float dt);

	cgp::vec3 border(float u);


	// To generate
	cgp::mesh_drawable drawable;
	std::vector<cgp::vec3> refPoints;
	std::vector<cgp::vec3> movedRefPoints;

	// To move
	cgp::rotation_transform rotation;
	cgp::vec3 translation;
	cgp::vec3 translation_p  = cgp::vec3(0.0f, 0.0f, 0.0f);
	cgp::vec3 translation_pp = cgp::vec3(0.0f, 0.0f, 0.0f);
	float last_t = 0;
	float m = 20;
	float K = 5;

	// To show
	float scaling = 4.0f;
	int N_triangles;
	int N_triangles_desired = 5;
};