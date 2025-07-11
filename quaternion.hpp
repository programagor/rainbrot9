#ifndef QUATERNION_HPP
#define QUATERNION_HPP

#include <mpfr.h>

struct Quaternion {
    mpfr_t r, i, j, k;  // Real and imaginary components

    Quaternion() {
        printf("Entering Quaternion constructor\n");
        mpfr_init(r); printf("Initialized r. Address: %p\n", (void*)r);
        mpfr_init(i); printf("Initialized i. Address: %p\n", (void*)i);
        mpfr_init(j); printf("Initialized j. Address: %p\n", (void*)j);
        mpfr_init(k); printf("Initialized k. Address: %p\n", (void*)k);
        
    }

    Quaternion(const char* r_str, const char* i_str, const char* j_str, const char* k_str, int base = 10) {
        printf("Entering Quaternion constructor with 4 strings\n");
        mpfr_init_set_str(r, r_str, base, MPFR_RNDN);
        mpfr_init_set_str(i, i_str, base, MPFR_RNDN);
        mpfr_init_set_str(j, j_str, base, MPFR_RNDN);
        mpfr_init_set_str(k, k_str, base, MPFR_RNDN);
    }

    Quaternion(double r_d, double i_d, double j_d, double k_d) {
        printf("Entering Quaternion constructor with 4 doubles: %f %f %f %f\n", r_d, i_d, j_d, k_d);
        mpfr_init_set_d(r, r_d, MPFR_RNDN);
        mpfr_init_set_d(i, i_d, MPFR_RNDN);
        mpfr_init_set_d(j, j_d, MPFR_RNDN);
        mpfr_init_set_d(k, k_d, MPFR_RNDN);
    }

    ~Quaternion() {
        printf("Entering Quaternion destructor\n");
        mpfr_clear(r); printf("Cleared r. Address: %p\n", (void*)r);
        mpfr_clear(i); printf("Cleared i. Address: %p\n", (void*)i);
        mpfr_clear(j); printf("Cleared j. Address: %p\n", (void*)j);
        mpfr_clear(k); printf("Cleared k. Address: %p\n", (void*)k);
    }

    Quaternion(const Quaternion& q) {
        printf("Entering Quaternion copy constructor\n");
        mpfr_init_set(r, q.r, MPFR_RNDN);
        mpfr_init_set(i, q.i, MPFR_RNDN);
        mpfr_init_set(j, q.j, MPFR_RNDN);
        mpfr_init_set(k, q.k, MPFR_RNDN);
        fprintf(stderr, "WARNING: Quaternion copy constructor called. Create new quaternions explicitly.\n");
    }

    Quaternion& operator=(const Quaternion& q) {
        printf("Entering Quaternion assignment operator\n");
        mpfr_set(r, q.r, MPFR_RNDN);
        mpfr_set(i, q.i, MPFR_RNDN);
        mpfr_set(j, q.j, MPFR_RNDN);
        mpfr_set(k, q.k, MPFR_RNDN);
        return *this;
    }

    // Function that takes 4 strings and sets the values of the quaternion
    void set(const char* r_str, const char* i_str, const char* j_str, const char* k_str, int base = 10) {
        printf("Entering Quaternion set with 4 strings: %s %s %s %s\n", r_str, i_str, j_str, k_str);
        mpfr_set_str(r, r_str, base, MPFR_RNDN);
        mpfr_set_str(i, i_str, base, MPFR_RNDN);
        mpfr_set_str(j, j_str, base, MPFR_RNDN);
        mpfr_set_str(k, k_str, base, MPFR_RNDN);
        printf("Finished set with 4 strings: %s %s %s %s\n", r_str, i_str, j_str, k_str);
    }

    // Function that takes 4 doubles and sets the values of the quaternion
    void set(double r_d, double i_d, double j_d, double k_d) {
        printf("Entering Quaternion set with 4 doubles: %f %f %f %f\n", r_d, i_d, j_d, k_d);
        mpfr_set_d(r, r_d, MPFR_RNDN);
        mpfr_set_d(i, i_d, MPFR_RNDN);
        mpfr_set_d(j, j_d, MPFR_RNDN);
        mpfr_set_d(k, k_d, MPFR_RNDN);
    }

    // Function that returns the values of the quaternion as strings
    void get(char* r_str, char* i_str, char* j_str, char* k_str, int precision = 10) {
        printf("Entering Quaternion get with precision %d\n", precision);
        mpfr_sprintf(r_str, "%.*Rf", precision, r);
        mpfr_sprintf(i_str, "%.*Rf", precision, i);
        mpfr_sprintf(j_str, "%.*Rf", precision, j);
        mpfr_sprintf(k_str, "%.*Rf", precision, k);
    }

    // Function that returns the values of the quaternion as doubles
    void get(double* r_d, double* i_d, double* j_d, double* k_d) {
        *r_d = mpfr_get_d(r, MPFR_RNDN);
        *i_d = mpfr_get_d(i, MPFR_RNDN);
        *j_d = mpfr_get_d(j, MPFR_RNDN);
        *k_d = mpfr_get_d(k, MPFR_RNDN);
    }

    // Set all components to zero
    void zero() {
        mpfr_set_zero(r, 0);
        mpfr_set_zero(i, 0);
        mpfr_set_zero(j, 0);
        mpfr_set_zero(k, 0);
    }

    // out = this + other
    void add(const Quaternion& other, Quaternion& out) const {
        mpfr_add(out.r, r, other.r, MPFR_RNDN);
        mpfr_add(out.i, i, other.i, MPFR_RNDN);
        mpfr_add(out.j, j, other.j, MPFR_RNDN);
        mpfr_add(out.k, k, other.k, MPFR_RNDN);
    }

    // out = this * other
    void mul(const Quaternion& other, Quaternion& out) const {
        mpfr_t tmp;
        mpfr_init(tmp);

        // real part
        mpfr_mul(out.r, r, other.r, MPFR_RNDN);
        mpfr_mul(tmp, i, other.i, MPFR_RNDN);
        mpfr_sub(out.r, out.r, tmp, MPFR_RNDN);
        mpfr_mul(tmp, j, other.j, MPFR_RNDN);
        mpfr_sub(out.r, out.r, tmp, MPFR_RNDN);
        mpfr_mul(tmp, k, other.k, MPFR_RNDN);
        mpfr_sub(out.r, out.r, tmp, MPFR_RNDN);

        // i component
        mpfr_mul(out.i, r, other.i, MPFR_RNDN);
        mpfr_mul(tmp, i, other.r, MPFR_RNDN);
        mpfr_add(out.i, out.i, tmp, MPFR_RNDN);
        mpfr_mul(tmp, j, other.k, MPFR_RNDN);
        mpfr_sub(out.i, out.i, tmp, MPFR_RNDN);
        mpfr_mul(tmp, k, other.j, MPFR_RNDN);
        mpfr_add(out.i, out.i, tmp, MPFR_RNDN);

        // j component
        mpfr_mul(out.j, r, other.j, MPFR_RNDN);
        mpfr_mul(tmp, j, other.r, MPFR_RNDN);
        mpfr_add(out.j, out.j, tmp, MPFR_RNDN);
        mpfr_mul(tmp, k, other.i, MPFR_RNDN);
        mpfr_add(out.j, out.j, tmp, MPFR_RNDN);
        mpfr_mul(tmp, i, other.k, MPFR_RNDN);
        mpfr_sub(out.j, out.j, tmp, MPFR_RNDN);

        // k component
        mpfr_mul(out.k, r, other.k, MPFR_RNDN);
        mpfr_mul(tmp, k, other.r, MPFR_RNDN);
        mpfr_add(out.k, out.k, tmp, MPFR_RNDN);
        mpfr_mul(tmp, i, other.j, MPFR_RNDN);
        mpfr_add(out.k, out.k, tmp, MPFR_RNDN);
        mpfr_mul(tmp, j, other.i, MPFR_RNDN);
        mpfr_sub(out.k, out.k, tmp, MPFR_RNDN);

        mpfr_clear(tmp);
    }

    // |q|^2
    void norm_sq(mpfr_t out) const {
        mpfr_t tmp;
        mpfr_init(tmp);
        mpfr_mul(out, r, r, MPFR_RNDN);
        mpfr_mul(tmp, i, i, MPFR_RNDN);
        mpfr_add(out, out, tmp, MPFR_RNDN);
        mpfr_mul(tmp, j, j, MPFR_RNDN);
        mpfr_add(out, out, tmp, MPFR_RNDN);
        mpfr_mul(tmp, k, k, MPFR_RNDN);
        mpfr_add(out, out, tmp, MPFR_RNDN);
        mpfr_clear(tmp);
    }
};

#endif