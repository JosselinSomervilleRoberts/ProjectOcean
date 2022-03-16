#include "wave.hpp"

using namespace cgp;

float wave_arg(float t, float a, float w, vec2 K, float u, float v) {
    return w * t - (K[0] * u + K[1] * v);
}

vec3 compute_wave_pos(vec3 pos, float time, int N_waves, std::vector<wave_parameters>& waves, wind_parameters wind, float wave_exponant, perlin_noise_parameters& noise, int noiseOctave)
{
    vec2 X = vec2(pos.x, pos.y);
    float Z = pos[2];
    float u = pos.x;
    float v = pos.y;

    for (int k = 0; k < N_waves; k++) {
        float a = waves[k].amplitude;
        float w = waves[k].angular_velocity;
        vec2 K  = waves[k].K;
        a *= wind.magnitude * std::pow(std::fabs(dot(normalize(K), wind.direction)), wave_exponant) / (float)(std::sqrt(N_waves));
        float arg = wave_arg(time, a, w, K, u, v);

        X += a * waves[k].dir * sin(arg);
        Z += a * cos(arg);
    }
    if (noise.used) Z += wind.magnitude * noise_perlin(vec3(X, time), noise.amplitude, noiseOctave, noise.persistency, noise.frequency, noise.frequency_gain, noise.dilatation_space, noise.dilatation_time);
    return vec3(X[0], X[1], Z);
}