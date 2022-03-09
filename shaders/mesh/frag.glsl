#version 330 core

in struct fragment_data
{
    vec3 position;
    vec3 normal;
    vec3 color;
    vec2 uv;

	vec3 eye;
} fragment;

in float ecume;

layout(location=0) out vec4 FragColor;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform sampler2D image_texture;

#define PI 3.1415


// Lights
struct LightSourceDir {
	vec3 direction;
	float intensity;
	vec3 color;
};

uniform LightSourceDir lightsourcesDir[10];
uniform int nb_lightsourcesDir;

uniform float ecume_exposant = 0.3f;
uniform float ecume_threshold = 0.4f;


uniform vec3 color = vec3(1.0, 1.0, 1.0); // Unifor color of the object
uniform float alpha = 1.0f; // alpha coefficient
uniform float Ka = 0.0f; // Ambient coefficient
uniform float Kd = 1.0f; // Diffuse coefficient
uniform float Ks = 1.0f; // Specular coefficient
uniform float specular_exp = 5.0; // Specular exponent
uniform bool use_texture = true;
uniform bool texture_inverse_y = false;



// Source: Code MyRenderer from INF584 (adapted)
vec3 get_fd(vec3 albedo) {
	return albedo / PI;
}

vec3 get_fs(vec3 w0, vec3 wi, vec3 wh, vec3 n, vec3 albedo, float roughness, float metallicness, float spec_exp) {
	float alpha = roughness;
	float alpha2 = pow(alpha, 2);
		
	float n_wh2 = pow(max(0., dot(n, wh)), 2);
	float n_wi = dot(n, wi);
	float n_w0 = dot(n, w0);
	float wi_wh = max(0., dot(wi, wh));
	float D = alpha2 /(PI * pow(1 + (alpha2 - 1) * n_wh2, 2));
		
	vec3 F0 = albedo + (vec3(1.) - albedo) * metallicness;
	vec3 F = F0 - (vec3(1.) - F0) * pow(1. - wi_wh, spec_exp);
		
	float G1 = 2 * n_wi/(n_wi+sqrt(alpha2+(1-alpha2)*pow(n_wi,2)));
	float G2 = 2 * n_w0/(n_w0+sqrt(alpha2+(1-alpha2)*pow(n_w0,2)));
	float G = G1 * G2;
		
	vec3 fs = D*F*G/(4*n_wi*n_w0);
	return fs;
}

vec3 get_r(vec3 position, vec3 normal, vec3 lightDirection, float lightIntensity, vec3 lightColor, vec3 albedo, float roughness, float metallicness, float K_diffuse, float K_specular, float spec_exp) {
	vec3 w0 = - normalize(position);
	vec3 wi = normalize(lightDirection);
	vec3 wh = normalize(wi + w0);

	vec3 n = normalize(normal);
	vec3 fs = get_fs(w0, wi, wh, n, albedo, roughness, metallicness, spec_exp);
	vec3 fd = get_fd(albedo);

	float scalarProd = max(0.0, dot(n, wi));
	vec3 luminosity = lightIntensity * lightColor;
		
	return luminosity * (K_diffuse*fd + K_specular*fs) * scalarProd;
}

void main()
{
	// Compute Normal
	vec3 N = normalize(fragment.normal);
	if (gl_FrontFacing == false) {
		N = -N;
	}

	// COLOR of the fragment 
	vec2 uv_image = vec2(fragment.uv.x, 1.0-fragment.uv.y);
	if(texture_inverse_y) {
		uv_image.y = 1.0-uv_image.y;
	}
	vec4 color_image_texture = texture(image_texture, uv_image);
	if(use_texture==false) {
		color_image_texture=vec4(1.0,1.0,1.0,1.0);
	}
	vec3 color_object  = fragment.color * color * color_image_texture.rgb;

	// PBR Shading
	vec3 pbr_color = vec3(0);
	for(int i=0; i<nb_lightsourcesDir; i++) {
		vec3 lightDirection = normalize(vec3(model * vec4(lightsourcesDir[i].direction, 1.0)));
		vec3 albedo = color_object;
		float roughness = 0.8f;
		float metallicness = 0.5f;
		pbr_color += get_r(fragment.position, N, -lightDirection, lightsourcesDir[i].intensity, lightsourcesDir[i].color, albedo, roughness, metallicness, Kd, Ks, specular_exp);
	}


	vec3 color_shading = (Ka * color_object) + pbr_color;
	float aaa = 0.6f;
	FragColor = vec4(color_shading, alpha * aaa * color_image_texture.a);
	

	if (ecume > ecume_threshold) {
		float coef = pow((ecume - ecume_threshold) / (1.0f - ecume_threshold), ecume_exposant);
		vec3 v = color_shading * (1.0f - coef) + coef * vec3(1,1,1);
		FragColor = vec4(v, ecume);
	}
}