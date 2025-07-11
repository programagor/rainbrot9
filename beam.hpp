#ifndef BEAM_HPP
#define BEAM_HPP

#include "quaternion.hpp"
#include <gmp.h>
#include <mpfr.h>

class Beam {
public:
    Quaternion mu;
    Quaternion sigma;
    int samples_total;
    int samples_current;
    gmp_randstate_t state_current;
    mpz_t seed_start;

    Beam();
    ~Beam();

    void get_sample(Quaternion& q);
};

#endif // BEAM_HPP
