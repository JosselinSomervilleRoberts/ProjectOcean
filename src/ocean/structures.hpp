#pragma once

#include "cgp/cgp.hpp"

struct wave_parameters {
	float amplitude;
	float angular_velocity;
	cgp::vec2 K;
	cgp::vec2 dir;
};

struct perlin_noise_parameters {
	bool used;
	float amplitude;
	int octave;
	float persistency;
	float frequency;
	float frequency_gain;
	float dilatation_space;
	float dilatation_time;
};

struct wind_parameters {
	float magnitude = 0.0f;
	cgp::vec2 direction = { 0,-1 };
};

struct LightSourceDir {
	cgp::vec3 direction;
	float intensity;
	cgp::vec3 color;
};