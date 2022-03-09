#pragma once

#include "cgp/cgp.hpp"
#include <math.h>
#include "noise.hpp"
#include "ocean/structures.hpp"

float wave_arg(float time, float a, float f, cgp::vec2 dir, float u, float v);

cgp::vec3 compute_wave_pos(cgp::vec3 pos, float time, int N_waves, std::vector<wave_parameters>& waves, perlin_noise_parameters& noise);