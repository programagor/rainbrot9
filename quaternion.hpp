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

    mpfr_ptr component(int idx) {
        switch(idx) {
            case 0: return r; break;
            case 1: return i; break;
            case 2: return j; break;
            default: return k;
        }
    }

    mpfr_srcptr component(int idx) const {
        switch(idx) {
            case 0: return r; break;
            case 1: return i; break;
            case 2: return j; break;
            default: return k;
        }
    }
};

inline void quaternion_add(const Quaternion& a, const Quaternion& b, Quaternion& res) {
    mpfr_add(res.r, a.r, b.r, MPFR_RNDN);
    mpfr_add(res.i, a.i, b.i, MPFR_RNDN);
    mpfr_add(res.j, a.j, b.j, MPFR_RNDN);
    mpfr_add(res.k, a.k, b.k, MPFR_RNDN);
}

inline void quaternion_mul(const Quaternion& a, const Quaternion& b, Quaternion& res) {
    mpfr_t t1, t2;
    mpfr_inits(t1, t2, (mpfr_ptr)0);

    // r component
    mpfr_mul(t1, a.r, b.r, MPFR_RNDN);
    mpfr_mul(t2, a.i, b.i, MPFR_RNDN);
    mpfr_sub(t1, t1, t2, MPFR_RNDN);
    mpfr_mul(t2, a.j, b.j, MPFR_RNDN);
    mpfr_sub(t1, t1, t2, MPFR_RNDN);
    mpfr_mul(t2, a.k, b.k, MPFR_RNDN);
    mpfr_sub(res.r, t1, t2, MPFR_RNDN);

    // i component
    mpfr_mul(t1, a.r, b.i, MPFR_RNDN);
    mpfr_mul(t2, a.i, b.r, MPFR_RNDN);
    mpfr_add(t1, t1, t2, MPFR_RNDN);
    mpfr_mul(t2, a.j, b.k, MPFR_RNDN);
    mpfr_add(t1, t1, t2, MPFR_RNDN);
    mpfr_mul(t2, a.k, b.j, MPFR_RNDN);
    mpfr_sub(res.i, t1, t2, MPFR_RNDN);

    // j component
    mpfr_mul(t1, a.r, b.j, MPFR_RNDN);
    mpfr_mul(t2, a.i, b.k, MPFR_RNDN);
    mpfr_sub(t1, t1, t2, MPFR_RNDN);
    mpfr_mul(t2, a.j, b.r, MPFR_RNDN);
    mpfr_add(t1, t1, t2, MPFR_RNDN);
    mpfr_mul(t2, a.k, b.i, MPFR_RNDN);
    mpfr_add(res.j, t1, t2, MPFR_RNDN);

    // k component
    mpfr_mul(t1, a.r, b.k, MPFR_RNDN);
    mpfr_mul(t2, a.i, b.j, MPFR_RNDN);
    mpfr_add(t1, t1, t2, MPFR_RNDN);
    mpfr_mul(t2, a.j, b.i, MPFR_RNDN);
    mpfr_sub(t1, t1, t2, MPFR_RNDN);
    mpfr_mul(t2, a.k, b.r, MPFR_RNDN);
    mpfr_add(res.k, t1, t2, MPFR_RNDN);

    mpfr_clears(t1, t2, (mpfr_ptr)0);
}

inline double quaternion_norm2(const Quaternion& q) {
    mpfr_t sum, tmp;
    mpfr_inits(sum, tmp, (mpfr_ptr)0);
    mpfr_mul(sum, q.r, q.r, MPFR_RNDN);
    mpfr_mul(tmp, q.i, q.i, MPFR_RNDN);
    mpfr_add(sum, sum, tmp, MPFR_RNDN);
    mpfr_mul(tmp, q.j, q.j, MPFR_RNDN);
    mpfr_add(sum, sum, tmp, MPFR_RNDN);
    mpfr_mul(tmp, q.k, q.k, MPFR_RNDN);
    mpfr_add(sum, sum, tmp, MPFR_RNDN);
    double result = mpfr_get_d(sum, MPFR_RNDN);
    mpfr_clears(sum, tmp, (mpfr_ptr)0);
    return result;
}

#endif