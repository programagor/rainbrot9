#include "beam.hpp"
#include <cstdio>

Beam::Beam() : samples_total(0), samples_current(0), max_iterations(64), escape_radius(4.0) {
    mu.set(0.0, 0.0, 0.0, 0.0);
    sigma.set(1.0, 1.0, 1.0, 1.0);
    gmp_randinit_default(state_current);
    mpz_init(seed_start);
}

Beam::~Beam() {
    mpz_clear(seed_start);
    gmp_randclear(state_current);
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

    mpfr_t tmp;
    mpfr_init(tmp);
    // q = mu + vars * sigma
    mpfr_mul(tmp, vars.r, sigma.r, MPFR_RNDN);
    mpfr_add(q.r, mu.r, tmp, MPFR_RNDN);

    mpfr_mul(tmp, vars.i, sigma.i, MPFR_RNDN);
    mpfr_add(q.i, mu.i, tmp, MPFR_RNDN);

    mpfr_mul(tmp, vars.j, sigma.j, MPFR_RNDN);
    mpfr_add(q.j, mu.j, tmp, MPFR_RNDN);

    mpfr_mul(tmp, vars.k, sigma.k, MPFR_RNDN);
    mpfr_add(q.k, mu.k, tmp, MPFR_RNDN);
    mpfr_clear(tmp);
    samples_current++;
}

bool Beam::iterate_seed(const Quaternion& c, Quaternion& hit_point, int& iter) {
    Quaternion z;
    for(iter = 0; iter < max_iterations; ++iter) {
        Quaternion z2;
        quaternion_mul(z, z, z2);
        quaternion_add(z2, c, z);
        double norm2 = quaternion_norm2(z);
        if(norm2 > escape_radius * escape_radius) {
            hit_point = z;
            return true;
        }
    }
    hit_point = z;
    return false;
}
