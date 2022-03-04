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


float wave_arg(float a, float f, vec2 dir, float u, float v) {
    return f*t - (dir[0]*u + dir[1]*v);
}

float wave_d_arg_du(float a, float f, vec2 dir, float u, float v) {
    return - dir[0];
}

float wave_d_arg_dv(float a, float f, vec2 dir, float u, float v) {
    return - dir[1];
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

    return vec3(X[0], X[1], Z);
}

vec3 compute_wave_norm(vec3 pos, vec3 norm)
{
    vec2  grad_u_xy = vec2(1.0f, 0.0f);
    float grad_u_z = 0.0f;
    vec2  grad_v_xy = vec2(0.0f, 1.0f);
    float grad_v_z = 0.0f;

    for(int k=0; k<N_waves; k++){
        float a  = waves[k].amplitude;
        float f  = waves[k].frequency;
        vec2 dir = waves[k].direction;
        float u = pos.x;
        float v = pos.y;
        float arg = wave_arg(a, f, dir, u, v);

        float d_arg_du = wave_d_arg_du(a, f, dir, u, v);
        grad_u_xy += - d_arg_du * a * normalize(dir) * cos(arg);
        grad_u_z  += + d_arg_du * a * sin(arg);
        
        float d_arg_dv = wave_d_arg_dv(a, f, dir, u, v);
        grad_v_xy += - d_arg_dv * a * normalize(dir) * cos(arg);
        grad_v_z  += + d_arg_dv * a * sin(arg);
    }

    vec3 grad_u = vec3(grad_u_xy.x, grad_u_xy.y, grad_u_z);
    vec3 grad_v = vec3(grad_v_xy.x, grad_v_xy.y, grad_v_z);

    return normalize(cross(grad_u, grad_v));
}

void main()
{
    waves[0] = Wave(1.0f, 3.1415f, normalize(vec2(1.0f, 0.2f)));

    vec3 pos = compute_wave_pos(position);
    vec3 norm = compute_wave_norm(position, normal);

	fragment.position = vec3(model * vec4(pos,1.0));
	fragment.normal   = vec3(model * vec4(norm  ,0.0));
	fragment.color = color;
	fragment.uv = uv;
	fragment.eye = vec3(inverse(view)*vec4(0,0,0,1.0));

	gl_Position = projection * view * model * vec4(pos, 1.0);
}


