#pragma once

#include "cgp/cgp.hpp"
#include "ocean/ocean.hpp"

class Ship {
public:

	Ship();
	void initialize();
	void draw(cgp::scene_environment_basic const& environment);

	void update(Ocean& ocean, float time);
	void computeRotation();
	void computeTranslation();


	// To generate
	cgp::mesh_drawable drawable;
	std::vector<cgp::vec3> refPoints;
	std::vector<cgp::vec3> movedRefPoints;

	// To move
	cgp::rotation_transform rotation;
	cgp::vec3 translation;
};