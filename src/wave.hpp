#pragma once

#include "cgp/cgp.hpp"
#include <math.h>
#include "noise.hpp"
#include "ocean/structures.hpp"

float wave_arg(float t, float a, float w, cgp::vec2 K, float u, float v);

cgp::vec3 compute_wave_pos(cgp::vec3 pos, float time, int N_waves, std::vector<wave_parameters>& waves, wind_parameters wind, float wave_exponant, perlin_noise_parameters& noise, int noiseOctave);