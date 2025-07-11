#ifndef PLATE_HPP
#define PLATE_HPP

#include <vector>
#include <cstdint>
#include <mpfr.h>
#include <mutex>
#include <memory>
#include "quaternion.hpp"

class Plate {
public:
    // Simple 4D->2D projection matrix
    mpfr_t projection[2][4];

    int width, height;
    std::vector<std::vector<int64_t>> data;       // energy buffer
    std::vector<std::unique_ptr<std::mutex>> row_mutexes; // row locks
    bool preview_open;

    Plate(int w, int h);
    ~Plate();

    // Project quaternion through projection matrix and add energy to pixel
    void insertPhoton(const Quaternion& q, int energy);
};

#endif // PLATE_HPP
