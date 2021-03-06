#include "ship.hpp"
#include "../scene.hpp"

using namespace cgp;
#define PI 3.14159f

Ship::Ship() {}

void Ship::initialize() {
	drawable.initialize(mesh_load_file_obj("assets/ship_translated.obj"));
	drawable.shader = opengl_load_shader("shaders/ship/vert.glsl", "shaders/ship/frag.glsl");
	drawable.texture = opengl_load_texture_image("assets/ship.jpg");
}

void Ship::draw(Scene& scene, bool display)
{
	drawable.shading.phong.specular = 0;
	drawable.shading.phong.diffuse = scene.getDiffuse();
	drawable.shading.phong.ambient = scene.getAmbient();
	drawable.shading.use_texture = scene.getUseTexture();
	drawable.shading.phong.specular_exponent = scene.getSpecularExponant();

	drawable.transform.rotation = rotation;
	drawable.transform.translation = translation;
	drawable.transform.scaling = scaling;

	cgp::scene_environment_basic const& environment = scene.environment;

	if (drawable.number_triangles == 0) return;

	// Setup shader
	assert_cgp(drawable.shader != 0, "Try to draw mesh_drawable without shader [name:" + drawable.name + "]");
	assert_cgp(drawable.texture != 0, "Try to draw mesh_drawable without texture [name:" + drawable.name + "]");
	glUseProgram(drawable.shader); opengl_check;

	// Send uniforms for this shader
	opengl_uniform(drawable.shader, environment);
	opengl_uniform(drawable.shader, drawable.shading);
	opengl_uniform(drawable.shader, "model", drawable.model_matrix());

	// Data
	scene.send_lights_to_GPU(drawable.shader);

	// Set texture
	glActiveTexture(GL_TEXTURE0); opengl_check;
	glBindTexture(GL_TEXTURE_2D, drawable.texture); opengl_check;
	opengl_uniform(drawable.shader, "image_texture", 0);  opengl_check;

	// Call draw function
	assert_cgp(drawable.number_triangles > 0, "Try to draw mesh_drawable with 0 triangles [name:" + drawable.name + "]"); opengl_check;
	glBindVertexArray(drawable.vao);   opengl_check;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drawable.vbo.at("index")); opengl_check;
	if (display) glDrawElements(GL_TRIANGLES, GLsizei(drawable.number_triangles * 3), GL_UNSIGNED_INT, nullptr); opengl_check;

	// Clean buffers
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	//if (show_wireframe) draw_wireframe(drawable, environment);
}

void Ship::computeRefPoints() {
	refPoints.resize(3 * N_triangles);
	movedRefPoints.resize(3 * N_triangles);

	for (int i = 0; i < N_triangles; i++) {
		refPoints[3 * i] = vec3(0, 0, -1.2f * scaling);
		refPoints[3 * i + 1] = border(2 * PI / N_triangles * i);
		refPoints[3 * i + 2] = border(2 * PI / N_triangles * (i + 1));
	}
}

void Ship::update(Ocean& ocean, float time) {
	if (N_triangles != N_triangles_desired) {
		N_triangles = N_triangles_desired;
		computeRefPoints();
	}

	for (int i = 0; i < 3 * N_triangles; i++) {
		movedRefPoints[i] = ocean.getVertexPos(refPoints[i], time, nb_octaves);
	}

	float dt = time - last_t;
	if (dt > 0) {
		computeRotation(dt);
		computeTranslation(dt);
	}
	last_t = time;
}


void Ship::computeRotation(float dt) {
	cgp::quaternion q = cgp::quaternion(0,0,0,0);

	for (int i = 0; i < 1; i++) {
		vec3 e1 = normalize(refPoints[3 * i + 2] - refPoints[3 * i + 1]);
		vec3 e2 = normalize(- refPoints[3 * i + 0] + 0.5f * (refPoints[3 * i + 1] + refPoints[3 * i + 2]));
		e2 -= dot(e1, e2) * e1;
		e2 = normalize(e2);
		vec3 e1_target = normalize(movedRefPoints[3 * i + 2] - movedRefPoints[3 * i + 1]);
		vec3 e2_target = normalize(- movedRefPoints[3 * i + 0] + 0.5f * (movedRefPoints[3 * i + 1] + movedRefPoints[3 * i + 2]));
		e2_target -= dot(e1_target, e2_target) * e1_target;
		e2_target = normalize(e2_target);
		q += (rotation_transform::between_vector(e1, e2, e1_target, e2_target)).quat();
	}

	q /= N_triangles;

	// Compute the speed metrics
	cgp::quaternion last_q = rotation.quat();

	// Compute the associated force
	cgp::quaternion q_p = (q - last_q) / dt;

	q_p /= std::max(1.0f, m * sqrt(1 + K));
	q = last_q + q_p * dt;

	rotation = rotation_transform::from_quaternion(normalize(q));
}

void Ship::computeTranslation(float dt) {

	// Compute the translation based on the wave movement
	cgp::vec3 trans = vec3(0, 0, 0);
	for (int i = 0; i < N_triangles; i++) {
		vec3 prevMid = (refPoints[3 * i + 0] + refPoints[3 * i + 1] + refPoints[3 * i + 2]) / 3.0f;
		vec3 newMid = (movedRefPoints[3 * i + 0] + movedRefPoints[3 * i + 1] + movedRefPoints[3 * i + 2]) / 3.0f;
		trans += newMid - prevMid;
	}
	trans /= N_triangles;

	// Compute the speed metrics
	cgp::vec3 trans_p  = (trans - translation) / dt;
	cgp::vec3 trans_pp = (trans_p - translation_p) / dt;

	// Compute the associated force
	cgp::vec3 force = trans_pp * m;

	// Compute the friction force
	cgp::vec3 forceFriction = -K * translation_p;

	// Compute the associated accel, speed, etc..
	translation_pp = (force + forceFriction) / m;
	cgp::vec3 new_speed = translation_p + translation_pp * dt;
	new_speed /= m * sqrt(1 + K);
	if (dot(translation_p, new_speed) < 0) // To prevent divergence
		new_speed = vec3(0, 0, 0);// -= dot(new_speed, speed) * speed / (pow(norm(speed), 2) * norm(new_speed));
	translation += new_speed * dt;

	// Update time
	translation_p = new_speed;

}

cgp::vec3 Ship::border(float u) {
	float Rx = 4 * scaling;
	float Ry = 1.3 * scaling;
	return cgp::vec3(Rx * std::cos(u), Ry * std::sin(u), -1.2f * scaling);
}