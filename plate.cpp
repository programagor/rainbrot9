#include "plate.hpp"
#include <cstdint>

Plate::Plate(int w, int h) : width(w), height(h) {
    for (int i = 0; i < 5; ++i)
        for (int j = 0; j < 4; ++j)
            mpfr_init(projection4[i][j]);
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 3; ++j)
            mpfr_init(projection3[i][j]);
    data.resize(width, std::vector<int64_t>(height, 0));
}

Plate::~Plate() {
    for (int i = 0; i < 5; ++i)
        for (int j = 0; j < 4; ++j)
            mpfr_clear(projection4[i][j]);
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 3; ++j)
            mpfr_clear(projection3[i][j]);
}

void Plate::insertPhoton(const Quaternion& q, int energy) {
    (void)q; (void)energy;
}
