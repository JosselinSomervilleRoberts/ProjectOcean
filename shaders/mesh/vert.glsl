#version 330 core

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

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


struct Wave {
	float amplitude;
	float frequency;
	vec2 direction;
};

Wave waves[1];
int N_waves = 1;
uniform float t;




// ====================== PERLIN NOISE =========================== //
float PI = 3.1415f;
float screenWidth = 10.0f;

float rand(vec2 c){
	return fract(sin(dot(c.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

float noise(vec2 p, float freq ){
	float unit = screenWidth/freq;
	vec2 ij = floor(p/unit);
	vec2 xy = mod(p,unit)/unit;
	//xy = 3.*xy*xy-2.*xy*xy*xy;
	xy = .5*(1.-cos(PI*xy));
	float a = rand((ij+vec2(0.,0.)));
	float b = rand((ij+vec2(1.,0.)));
	float c = rand((ij+vec2(0.,1.)));
	float d = rand((ij+vec2(1.,1.)));
	float x1 = mix(a, b, xy.x);
	float x2 = mix(c, d, xy.x);
	return mix(x1, x2, xy.y);
}

float pNoise(vec2 p, int res){
	float persistance = .5;
	float n = 0.;
	float normK = 0.;
	float f = 4.;
	float amp = 1.;
	int iCount = 0;
	for (int i = 0; i<50; i++){
		n+=amp*noise(p, f);
		f*=2.;
		normK+=amp;
		amp*=persistance;
		if (iCount == res) break;
		iCount++;
	}
	float nf = n/normK;
	return nf*nf*nf*nf;
}
// =================================================================== //


float wave_arg(float a, float f, vec2 dir, float u, float v) {
    return f*t - (dir[0]*u + dir[1]*v);
}

vec3 compute_wave_pos(vec3 pos)
{
    vec2 X = vec2(pos);
    float Z = pos[2];

    for(int k=0; k<N_waves; k++){
        float a  = waves[k].amplitude;
        float f  = waves[k].frequency;
        vec2 dir = waves[k].direction;
        float u = pos.x;
        float v = pos.y;
        float arg = wave_arg(a, f, dir, u, v);
        X += a * normalize(dir) * sin(arg);
        Z += a * cos(arg);
    }
	Z+= pNoise(vec2(position), 1);

    return vec3(X[0], X[1], Z);
}

vec3 du(float u, float v) {
    float EPSILON = 0.001f;
    vec3 v1 = compute_wave_pos(vec3(u + EPSILON, v, 0));
    vec3 v2 = compute_wave_pos(vec3(u - EPSILON, v, 0));
    return (v2 - v1) / (2.0f * EPSILON);
}

vec3 dv(float u, float v) {
    float EPSILON = 0.001f;
    vec3 v1 = compute_wave_pos(vec3(u, v + EPSILON, 0));
    vec3 v2 = compute_wave_pos(vec3(u, v - EPSILON, 0));
    return (v2 - v1) / (2.0f * EPSILON);
}

vec3 compute_wave_norm(vec3 pos)
{
    vec3 grad_u = du(pos.x, pos.y);
    vec3 grad_v = dv(pos.x, pos.y);

    return normalize(cross(grad_u, grad_v));
}

void main()
{
    waves[0] = Wave(1.0f, 3.1415f, normalize(vec2(1.0f, 0.2f)));

    vec3 pos = compute_wave_pos(position);
    vec3 norm = compute_wave_norm(position);

	fragment.position = vec3(model * vec4(pos,1.0));
	fragment.normal   = vec3(model * vec4(norm  ,0.0));
	fragment.color = color;
	fragment.uv = uv;
	fragment.eye = vec3(inverse(view)*vec4(0,0,0,1.0));

	gl_Position = projection * view * model * vec4(pos, 1.0);
}


