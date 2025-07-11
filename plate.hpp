#ifndef PLATE_HPP
#define PLATE_HPP

#include <vector>
#include <cstdint>
#include <mpfr.h>
#include "quaternion.hpp"

class Plate {
public:
    mpfr_t projection4[5][4];
    mpfr_t projection3[4][3];
    int width, height;
    std::vector<std::vector<int64_t>> data;

    Plate(int w, int h);
    ~Plate();

    void insertPhoton(const Quaternion& q, int energy);
};

#endif // PLATE_HPP
