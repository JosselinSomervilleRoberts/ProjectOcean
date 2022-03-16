#version 330 core
#define PI 3.14159f
#define EPSILON 0.01f
#define INV_EPSILON 100.0f

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 color;
layout (location = 3) in vec2 uv;

out struct fragment_data
{
    vec3 position;
    vec3 normal;
    vec3 color;
    vec2 uv;
	vec3 eye;
} fragment;

out float coeff_seafoam;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


struct Wave {
	float amplitude;
	float angular_velocity;
	vec2 K;
    vec2 dir;
};
uniform Wave waves[100];
uniform int N_waves;


struct PerlinNoise {
    bool used;
	float amplitude;
    int octave;
	float persistency;
	float frequency;
	float frequency_gain;
  	float dilatation_space;
    float dilatation_time;
};
uniform PerlinNoise noise;

struct SeaFoam {
    float exponent;
    float render_threshold;
    float compute_threshold;
    float ha;
    float alpha_g;
    float h0;
    float hepsilon;
    int nbOctaves;
    vec3 color;
    bool display;
    bool only;
};
uniform SeaFoam foam;

uniform float t;




// ====================== PERLIN NOISE =========================== //
// Source: snoise3 : https://gist.github.com/patriciogonzalezvivo/670c22f3966e662d2f83
// Perlin Noise: adpatation of CGP 3d Perlin Noise
vec4 permute(vec4 x){return mod(((x*34.0)+1.0)*x, 289.0);}
vec4 taylorInvSqrt(vec4 r){return 1.79284291400159 - 0.85373472095314 * r;}

float snoise3(vec3 v){ 
  const vec2  C = vec2(1.0/6.0, 1.0/3.0) ;
  const vec4  D = vec4(0.0, 0.5, 1.0, 2.0);

// First corner
  vec3 i  = floor(v + dot(v, C.yyy) );
  vec3 x0 =   v - i + dot(i, C.xxx) ;

// Other corners
  vec3 g = step(x0.yzx, x0.xyz);
  vec3 l = 1.0 - g;
  vec3 i1 = min( g.xyz, l.zxy );
  vec3 i2 = max( g.xyz, l.zxy );

  //  x0 = x0 - 0. + 0.0 * C 
  vec3 x1 = x0 - i1 + 1.0 * C.xxx;
  vec3 x2 = x0 - i2 + 2.0 * C.xxx;
  vec3 x3 = x0 - 1. + 3.0 * C.xxx;

// Permutations
  i = mod(i, 289.0 ); 
  vec4 p = permute( permute( permute( 
             i.z + vec4(0.0, i1.z, i2.z, 1.0 ))
           + i.y + vec4(0.0, i1.y, i2.y, 1.0 )) 
           + i.x + vec4(0.0, i1.x, i2.x, 1.0 ));

// Gradients
// ( N*N points uniformly over a square, mapped onto an octahedron.)
  float n_ = 1.0/7.0; // N=7
  vec3  ns = n_ * D.wyz - D.xzx;

  vec4 j = p - 49.0 * floor(p * ns.z *ns.z);  //  mod(p,N*N)

  vec4 x_ = floor(j * ns.z);
  vec4 y_ = floor(j - 7.0 * x_ );    // mod(j,N)

  vec4 x = x_ *ns.x + ns.yyyy;
  vec4 y = y_ *ns.x + ns.yyyy;
  vec4 h = 1.0 - abs(x) - abs(y);

  vec4 b0 = vec4( x.xy, y.xy );
  vec4 b1 = vec4( x.zw, y.zw );

  vec4 s0 = floor(b0)*2.0 + 1.0;
  vec4 s1 = floor(b1)*2.0 + 1.0;
  vec4 sh = -step(h, vec4(0.0));

  vec4 a0 = b0.xzyw + s0.xzyw*sh.xxyy ;
  vec4 a1 = b1.xzyw + s1.xzyw*sh.zzww ;

  vec3 p0 = vec3(a0.xy,h.x);
  vec3 p1 = vec3(a0.zw,h.y);
  vec3 p2 = vec3(a1.xy,h.z);
  vec3 p3 = vec3(a1.zw,h.w);

//Normalise gradients
  vec4 norm = taylorInvSqrt(vec4(dot(p0,p0), dot(p1,p1), dot(p2, p2), dot(p3,p3)));
  p0 *= norm.x;
  p1 *= norm.y;
  p2 *= norm.z;
  p3 *= norm.w;

// Mix final noise value
  vec4 m = max(0.6 - vec4(dot(x0,x0), dot(x1,x1), dot(x2,x2), dot(x3,x3)), 0.0);
  m = m * m;
  return 42.0 * dot( m*m, vec4( dot(p0,x0), dot(p1,x1), 
                                dot(p2,x2), dot(p3,x3) ) );
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
    for(int k=0;k<octave;k++)
    {
        float n = snoise3(vec3(p.x*f, p.y*f, p.z*f));
        value += a * (0.5f+0.5f*n);
        f *= frequency_gain;
        a *= persistency;
    }
    return amplitude * value;
}

// =================================================================== //


float wave_arg(float t, float a, float w, vec2 K, float u, float v) {
    return w * t - (K[0]*u + K[1]*v);
}

vec3 compute_wave_pos(vec3 pos, float time, int noiseOctave)
{
    vec2 X = vec2(pos);
    float Z = pos[2];
    float u = pos.x;
    float v = pos.y;

    for(int k=0; k < N_waves; k++){
        float a  = waves[k].amplitude;
        float w  = waves[k].angular_velocity;
        vec2 K   = waves[k].K;
        float arg = wave_arg(time, a, w, K, u, v);

        X += a * waves[k].dir * sin(arg);
        Z += a * cos(arg);
    }
    if (noise.used) Z += noise_perlin(vec3(X, time), noise.amplitude, noiseOctave, noise.persistency, noise.frequency, noise.frequency_gain, noise.dilatation_space, noise.dilatation_time);
    return vec3(X[0], X[1], Z);
}



vec3 compute_wave_norm(vec3 pos, vec3 fuv, float time, int noiseOctave)
{
    float u = pos.x;
    float v = pos.y;
    float z = pos.z;

    // Positions
    vec3 f1uv  = compute_wave_pos(vec3(u + EPSILON, v, z), time, noiseOctave);
    vec3 fu1v  = compute_wave_pos(vec3(u, v + EPSILON, z), time, noiseOctave);

    // Derivatives of positions
    vec3 dfuvx  = (f1uv - fuv) * INV_EPSILON;
    vec3 dfuvy  = (fu1v - fuv) * INV_EPSILON;

    // Normal
    vec3 nuv  = normalize(cross(dfuvx, dfuvy));
    return nuv;
}


float max_dn(float time, float u, float v, float z, int nbOctaves) {
    // Positions
    vec3 fuv   = compute_wave_pos(vec3(u, v, z), time, nbOctaves);
    vec3 f1uv  = compute_wave_pos(vec3(u + EPSILON, v, z), time, nbOctaves);
    vec3 f2uv  = compute_wave_pos(vec3(u + 2*EPSILON, v, z), time, nbOctaves);
    vec3 f1u1v = compute_wave_pos(vec3(u + EPSILON, v + EPSILON, z), time, nbOctaves);
    vec3 fu1v  = compute_wave_pos(vec3(u, v + EPSILON, z), time, nbOctaves);
    vec3 fu2v  = compute_wave_pos(vec3(u, v + 2*EPSILON, z), time, nbOctaves);
    
    // Derivatives of positions
    vec3 dfuvx  = (f1uv - fuv)   * INV_EPSILON;
    vec3 dfuvy  = (fu1v - fuv)   * INV_EPSILON;
    vec3 df1uvx = (f2uv - f1uv)  * INV_EPSILON;
    vec3 df1uvy = (f1u1v - f1uv) * INV_EPSILON;
    vec3 dfu1vy = (fu2v - fu1v)  * INV_EPSILON;
    vec3 dfu1vx = (f1u1v - fu1v) * INV_EPSILON;

    // Normals
    vec3 nuv  = normalize(cross(dfuvx, dfuvy));
    vec3 n1uv = normalize(cross(df1uvx, df1uvy));
    vec3 nu1v = normalize(cross(dfu1vx, dfu1vy));

    // Derivatives of normals
    vec3 dnx = (n1uv - nuv) * INV_EPSILON;
    vec3 dny = (nu1v - nuv) * INV_EPSILON;
    float dx = abs(dnx.z);
    float dy = abs(dny.z);

    // Compute directional derivative
    if(dx < EPSILON) return dy;
    float d1 = dy / dx;
    float d2 = d1 * d1;
    return dx * sqrt(1 + d2);
}

float compute_seafoam(float time, vec3 wave_pos, vec3 uv_pos, vec3 norm) {
    if (wave_pos.z > foam.compute_threshold) return min(1.0f, foam.render_threshold + foam.ha + wave_pos.z - foam.compute_threshold);
    float dn = max_dn(time, uv_pos.x, uv_pos.y, uv_pos.z, foam.nbOctaves);
    return min(1.0f, max(0, (foam.alpha_g*dn)) * min(1, wave_pos.z - foam.h0) * (1.0f + foam.hepsilon - norm.z));
}


void main()
{
    vec3 pos = compute_wave_pos(position, t, noise.octave);
    vec3 norm = compute_wave_norm(position, pos, t, noise.octave);

    fragment.position = vec3(model * vec4(pos,1.0));
    fragment.normal   = vec3(model * vec4(norm  ,0.0));
    fragment.color = color;
    fragment.uv = uv;
    fragment.eye = vec3(inverse(view)*vec4(0,0,0,1.0));

    gl_Position = projection * view * model * vec4(pos, 1.0);
    coeff_seafoam = 0;
    if (foam.display) coeff_seafoam = compute_seafoam(t, pos, position, norm);
}