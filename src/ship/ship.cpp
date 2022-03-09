#include "Ship.hpp"

using namespace cgp;

Ship::Ship() {}

void Ship::initialize() {
	drawable.initialize(mesh_load_file_obj("assets/ship_translated.obj"));
	drawable.texture = opengl_load_texture_image("assets/ship.jpg");
	drawable.shader = opengl_load_shader("shaders/ship/vert.glsl", "shaders/ship/frag.glsl");

	refPoints.resize(3);
	movedRefPoints.resize(3);
	refPoints[0] = vec3(15, 0, 0);
	refPoints[1] = vec3(-8, 4, 0);
	refPoints[2] = vec3(-8, -4, 0);
}

void Ship::draw(cgp::scene_environment_basic const& environment) {
	drawable.transform.rotation = rotation;
	drawable.transform.translation = translation;
	drawable.transform.scaling = 4.0f;
	cgp::draw(drawable, environment);
}

void Ship::update(Ocean& ocean, float time) {
	for (int i = 0; i < 3; i++) {
		movedRefPoints[i] = ocean.getVertexPos(refPoints[i], time);
	}
	computeRotation();
	computeTranslation();
}


void Ship::computeRotation() {
	vec3 e1 = normalize(refPoints[1] - refPoints[0]);
	vec3 e2 = normalize(refPoints[2] - 0.5f * (refPoints[1] + refPoints[0]));
	e2 -= dot(e1, e2)* e1;
	e2 = normalize(e2);
	vec3 e1_target = normalize(movedRefPoints[1] - movedRefPoints[0]);
	vec3 e2_target = normalize(movedRefPoints[2] - 0.5f * (movedRefPoints[1] + movedRefPoints[0]));
	e2_target -= dot(e1_target, e2_target) * e1_target;
	e2_target = normalize(e2_target);
	rotation = rotation_transform::between_vector(e1, e2, e1_target, e2_target);
}

void Ship::computeTranslation() {
	vec3 prevMid = (refPoints[0] + refPoints[1] + refPoints[2]) / 3.0f;
	vec3 newMid  = (movedRefPoints[0] + movedRefPoints[1] + movedRefPoints[2]) / 3.0f;
	translation = newMid - prevMid;
}