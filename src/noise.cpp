#include "noise.hpp"

using namespace cgp;

vec3 floor(vec3 v) { return vec3(floor(v.x), floor(v.y), floor(v.z)); }
vec4 floor(vec4 v) { return vec4(floor(v.x), floor(v.y), floor(v.z), floor(v.w)); }
vec3 xxx(vec2 v) { return vec3(v.x, v.x, v.x); }
vec3 xxx(vec4 v) { return vec3(v.x, v.x, v.x); }
vec3 yyy(vec2 v) { return vec3(v.y, v.y, v.y); }
vec4 yyyy(vec3 v) { return vec4(v.y, v.y, v.y, v.y); }
vec3 zxy(vec3 v) { return vec3(v.z, v.x, v.y); }
vec3 yzx(vec3 v) { return vec3(v.y, v.z, v.x); }
vec3 xzx(vec4 v) { return vec3(v.x, v.z, v.x); }
vec3 wyz(vec4 v) { return vec3(v.w, v.y, v.z); }
vec4 xzyw(vec4 v) { return vec4(v.x, v.z, v.y, v.w); }
vec4 xxyy(vec4 v) { return vec4(v.x, v.x, v.y, v.y); }
vec4 zzww(vec4 v) { return vec4(v.z, v.z, v.w, v.w); }
float mod(float x, float y) { return x - y * std::floor(x / y); } //https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/mod.xhtml
vec3 mod(vec3 v, float x) { return vec3(mod(v.x, x), mod(v.y, x), mod(v.z, x)); }
vec4 mod(vec4 v, float x) { return vec4(mod(v.x, x), mod(v.y, x), mod(v.z, x), mod(v.w, x)); }
vec3 step(vec3 edge, vec3 x) { return vec3((float)((int)(x.x >= edge.x)), (float)((int)(x.y >= edge.y)), (float)((int)(x.z >= edge.z))); } // https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/step.xhtml
vec4 step(vec4 edge, vec4 x) { return vec4((float)((int)(x.x >= edge.x)), (float)((int)(x.y >= edge.y)), (float)((int)(x.z >= edge.z)), (float)((int)(x.w >= edge.w))); }
vec4 max(vec4 x, vec4 y) { return vec4(std::max(x.x, y.x), std::max(x.y, y.y), std::max(x.z, y.z), std::max(x.w, y.w)); }
vec3 max(vec3 x, vec3 y) { return vec3(std::max(x.x, y.x), std::max(x.y, y.y), std::max(x.z, y.z)); }
vec3 min(vec3 x, vec3 y) { return vec3(std::min(x.x, y.x), std::min(x.y, y.y), std::min(x.z, y.z)); }

vec4 permute(vec4 x) { return mod(((x * 34.0) + vec4(1.0f, 1.0f, 1.0f, 1.0f)) * x, 289.0); }
vec4 taylorInvSqrt(vec4 r) { return vec4(1.79284291400159f, 1.79284291400159f, 1.79284291400159f, 1.79284291400159f) - 0.85373472095314 * r; }


float snoise3(vec3 v) {
    const vec2  C = vec2(1.0 / 6.0, 1.0 / 3.0);
    const vec4  D = vec4(0.0, 0.5, 1.0, 2.0);

    // First corner
    vec3 i = floor(v + dot(v, yyy(C)));
    vec3 x0 = v - i + dot(i, xxx(C));

    // Other corners
    vec3 g = step(yzx(x0), x0);
    vec3 l = vec3(1.0f, 1.0f, 1.0f) - g;
    vec3 i1 = min(g, zxy(l));
    vec3 i2 = max(g, zxy(l));

    //  x0 = x0 - 0. + 0.0 * C
    vec3 x1 = x0 - i1 + 1.0 * xxx(C);
    vec3 x2 = x0 - i2 + 2.0 * xxx(C);
    vec3 x3 = x0 - vec3(1.0f, 1.0f, 1.0f) + 3.0 * xxx(C);

    // Permutations
    i = mod(i, 289.0);
    vec4 p = permute(permute(permute(
        i.z + vec4(0.0, i1.z, i2.z, 1.0))
        + i.y + vec4(0.0, i1.y, i2.y, 1.0))
        + i.x + vec4(0.0, i1.x, i2.x, 1.0));

    // Gradients
    // ( N*N points uniformly over a square, mapped onto an octahedron.)
    float n_ = 1.0 / 7.0; // N=7
    vec3  ns = n_ * wyz(D) - xzx(D);

    vec4 j = p - 49.0 * floor(p * ns.z * ns.z);  //  mod(p,N*N)

    vec4 x_ = floor(j * ns.z);
    vec4 y_ = floor(j - 7.0 * x_);    // mod(j,N)

    vec4 x = x_ * ns.x + yyyy(ns);
    vec4 y = y_ * ns.x + yyyy(ns);
    vec4 h = vec4(1.0f, 1.0f, 1.0f, 1.0f) - abs(x) - abs(y);

    vec4 b0 = vec4(x.x, x.y, y.x, y.y);
    vec4 b1 = vec4(x.z, x.w, y.z, y.w);

    vec4 s0 = floor(b0) * 2.0 + vec4(1.0f, 1.0f, 1.0f, 1.0f);
    vec4 s1 = floor(b1) * 2.0 + vec4(1.0f, 1.0f, 1.0f, 1.0f);
    vec4 sh = - step(h, vec4(0.0f, 0.0f, 0.0f, 0.0f));

    vec4 a0 = xzyw(b0) + xzyw(s0) * xxyy(sh);
    vec4 a1 = xzyw(b1) + xzyw(s1) * zzww(sh);

    vec3 p0 = vec3(a0.x, a0.y, h.x);
    vec3 p1 = vec3(a0.z, a0.w, h.y);
    vec3 p2 = vec3(a1.x, a1.y, h.z);
    vec3 p3 = vec3(a1.z, a1.w, h.w);

    //Normalise gradients
    vec4 norm = taylorInvSqrt(vec4(dot(p0, p0), dot(p1, p1), dot(p2, p2), dot(p3, p3)));
    p0 *= norm.x;
    p1 *= norm.y;
    p2 *= norm.z;
    p3 *= norm.w;

    // Mix final noise value
    vec4 m = max(vec4(0.6f, 0.6f, 0.6f, 0.6f) - vec4(dot(x0, x0), dot(x1, x1), dot(x2, x2), dot(x3, x3)), vec4(0.0f, 0.0f, 0.0f, 0.0f));
    m = m * m;
    return 42.0 * dot(m * m, vec4(dot(p0, x0), dot(p1, x1),
        dot(p2, x2), dot(p3, x3)));
}

float noise_perlin(vec3 p, float amplitude, int octave, float persistency, float frequency, float frequency_gain, float dilatation_space, float dilatation_time)
{
    // dilatation
    p.x *= dilatation_space;
    p.y *= dilatation_space;
    p.z *= dilatation_time;

    float value = 0.0f;
    float a = 1.0f; // current magnitude
    float f = frequency; // current frequency
    for (int k = 0; k < octave; k++)
    {
        float n = snoise3(vec3(p.x * f, p.y * f, p.z * f));
        value += a * (0.5f + 0.5f * n);
        f *= frequency_gain;
        a *= persistency;
    }
    return amplitude * value;
}