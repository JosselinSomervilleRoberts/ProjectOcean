#include "simulation.hpp"

using namespace cgp;

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