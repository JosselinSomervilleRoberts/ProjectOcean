#include "wave.hpp"

using namespace cgp;

float wave_arg(float time, float f, vec2 dir, float u, float v) {
    return f * time - (dir[0] * u + dir[1] * v);
}

vec3 compute_wave_pos(vec3 pos, float time, int N_waves, std::vector<wave_parameters>& waves, perlin_noise_parameters& noise)
{
    vec2 X = vec2(pos.x, pos.y);
    float Z = pos[2];

    for (int k = 0; k < N_waves; k++) {
        float a = waves[k].amplitude;
        float w = waves[k].angular_velocity;
        vec2 K  = waves[k].K;
        float u = pos.x;
        float v = pos.y;
        float arg = wave_arg(time, w, K, u, v);

        X += a * waves[k].dir * sin(arg);
        Z += a * cos(arg);
    }
    if (noise.used) Z += noise_perlin(vec3(X, time), noise.amplitude, noise.octave, noise.persistency, noise.frequency, noise.frequency_gain, noise.dilatation_space, noise.dilatation_time);
    return vec3(X[0], X[1], Z);
}