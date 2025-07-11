#include "beam.hpp"
#include <cstdio>

Beam::Beam() : samples_total(0), samples_current(0) {
    mu.set(0.0, 0.0, 0.0, 0.0);
    sigma.set(0.0, 0.0, 0.0, 0.0);
    gmp_randinit_default(state_current);
    mpz_init(seed_start);
}

Beam::~Beam() {
    mpz_clear(seed_start);
}

void Beam::get_sample(Quaternion& q) {
    if(samples_current == 0) {
        gmp_randseed(state_current, seed_start);
    }
    Quaternion vars;
    mpfr_urandom(vars.r, state_current, MPFR_RNDN);
    mpfr_urandom(vars.i, state_current, MPFR_RNDN);
    mpfr_urandom(vars.j, state_current, MPFR_RNDN);
    mpfr_urandom(vars.k, state_current, MPFR_RNDN);
}
