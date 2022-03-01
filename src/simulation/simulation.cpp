#include "simulation.hpp"

using namespace cgp;
#define PI 3.1415f


void add_waves(waves_parameters& waves, size_t N, cgp::vec2 global_dir) {
    waves.amplitude.resize(N);
    waves.frequency.resize(N);
    waves.direction.resize(N);

    for (int i = 0; i < N; i++) {
        waves.frequency[i] = PI * (1 + 3*((double)rand() / (RAND_MAX)));
        float angle = PI * (0.5f + ((double)rand() / (RAND_MAX)));
        cgp::vec2 dir;
        dir.x = std::cos(angle) * global_dir.x + std::sin(angle) * global_dir.y;
        dir.y = std::cos(angle) * global_dir.y - std::sin(angle) * global_dir.x;
        waves.direction[i] = dir;
        waves.amplitude[i] = 10.0f * ((double)rand() / (RAND_MAX)) / (float)(N) * std::pow(std::fabs(cgp::dot(dir, global_dir)), 4.0f);
    }
}

void compute_vertex_position(ocean_structure &ocean, cgp::grid_2D<cgp::vec3> original_position, 
                             waves_parameters waves, float t){
    int const N = ocean.N_samples_edge();
    
    // waves' parameters
    cgp::buffer<float> a = waves.amplitude; 
    cgp::buffer<float> w = waves.frequency;
    cgp::buffer<cgp::vec2> K = waves.direction;
    int N_waves = a.size();
    
    for(int i=0; i<N; i++){
        for(int j=0; j<N; j++){
            cgp::vec3 init_position = original_position(i,j);
            cgp::vec2 X_0 = {init_position[0], init_position[1]};
            cgp::vec2 X = {init_position[0], init_position[1]}; // horizontal direction
            float Z = init_position[2];  // vertical direction
            
            for(int k=0; k<N_waves; k++){
                X += a[k] * normalize(K[k]) * std::sin(w[k]*t- (K[k][0]*X_0[0]+K[k][1]*X_0[1]));
                Z += a[k] * std::cos(w[k]*t- (K[k][0]*X_0[0]+K[k][1]*X_0[1]));
            }
            ocean.position(i,j).x = X[0];
            ocean.position(i,j).y = X[1];
            ocean.position(i,j).z = Z;
        }
    }

}