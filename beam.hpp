#ifndef BEAM_HPP
#define BEAM_HPP

#include "quaternion.hpp"
#include <gmp.h>
#include <mpfr.h>

class Beam {
public:
    Quaternion mu;               // Mean value for seed generation
    Quaternion sigma;            // Standard deviation for seed generation
    int samples_total;           // How many seeds to generate
    int samples_current;         // Counter of generated seeds
    gmp_randstate_t state_current; // Random state used for generation
    mpz_t seed_start;            // Initial seed for reproducibility

    int max_iterations;          // Fractal iteration cap
    double escape_radius;        // Escape radius for divergence

    Beam();
    ~Beam();

    // Generate a random seed quaternion distributed around mu,sigma
    void get_sample(Quaternion& q);

    // Iterate z = z^2 + c starting from z=0.
    // Returns true if the orbit escapes within max_iterations.
    bool iterate_seed(const Quaternion& c, Quaternion& hit_point, int& iter);
};

#endif // BEAM_HPP
