#pragma once

#include "cgp/cgp.hpp"
#include <math.h>

float snoise3(cgp::vec3 v);

float noise_perlin(cgp::vec3 p, float amplitude, int octave, float persistency, float frequency, float frequency_gain, float dilatation_space, float dilatation_time);