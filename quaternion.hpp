#ifndef QUATERNION_HPP
#define QUATERNION_HPP

#include <mpfr.h>

struct Quaternion {
    mpfr_t r, i, j, k;

    Quaternion() {
        mpfr_init(r);
        mpfr_init(i);
        mpfr_init(j);
        mpfr_init(k);
    }

    Quaternion(const char* r_str, const char* i_str, const char* j_str, const char* k_str, int base = 10) {
        mpfr_init_set_str(r, r_str, base, MPFR_RNDN);
        mpfr_init_set_str(i, i_str, base, MPFR_RNDN);
        mpfr_init_set_str(j, j_str, base, MPFR_RNDN);
        mpfr_init_set_str(k, k_str, base, MPFR_RNDN);
    }

    Quaternion(double r_d, double i_d, double j_d, double k_d) {
        mpfr_init_set_d(r, r_d, MPFR_RNDN);
        mpfr_init_set_d(i, i_d, MPFR_RNDN);
        mpfr_init_set_d(j, j_d, MPFR_RNDN);
        mpfr_init_set_d(k, k_d, MPFR_RNDN);
    }

    ~Quaternion() {
        mpfr_clear(r);
        mpfr_clear(i);
        mpfr_clear(j);
        mpfr_clear(k);
    }

    Quaternion(const Quaternion& q) {
        mpfr_init_set(r, q.r, MPFR_RNDN);
        mpfr_init_set(i, q.i, MPFR_RNDN);
        mpfr_init_set(j, q.j, MPFR_RNDN);
        mpfr_init_set(k, q.k, MPFR_RNDN);
    }

    Quaternion& operator=(const Quaternion& q) {
        mpfr_set(r, q.r, MPFR_RNDN);
        mpfr_set(i, q.i, MPFR_RNDN);
        mpfr_set(j, q.j, MPFR_RNDN);
        mpfr_set(k, q.k, MPFR_RNDN);
        return *this;
    }

    void set(const char* r_str, const char* i_str, const char* j_str, const char* k_str, int base = 10) {
        mpfr_set_str(r, r_str, base, MPFR_RNDN);
        mpfr_set_str(i, i_str, base, MPFR_RNDN);
        mpfr_set_str(j, j_str, base, MPFR_RNDN);
        mpfr_set_str(k, k_str, base, MPFR_RNDN);
    }

    void set(double r_d, double i_d, double j_d, double k_d) {
        mpfr_set_d(r, r_d, MPFR_RNDN);
        mpfr_set_d(i, i_d, MPFR_RNDN);
        mpfr_set_d(j, j_d, MPFR_RNDN);
        mpfr_set_d(k, k_d, MPFR_RNDN);
    }

    void get(char* r_str, char* i_str, char* j_str, char* k_str, int precision = 10) const {
        mpfr_sprintf(r_str, "%.*Rf", precision, r);
        mpfr_sprintf(i_str, "%.*Rf", precision, i);
        mpfr_sprintf(j_str, "%.*Rf", precision, j);
        mpfr_sprintf(k_str, "%.*Rf", precision, k);
    }

    void get(double* r_d, double* i_d, double* j_d, double* k_d) const {
        *r_d = mpfr_get_d(r, MPFR_RNDN);
        *i_d = mpfr_get_d(i, MPFR_RNDN);
        *j_d = mpfr_get_d(j, MPFR_RNDN);
        *k_d = mpfr_get_d(k, MPFR_RNDN);
    }
};

#endif // QUATERNION_HPP
