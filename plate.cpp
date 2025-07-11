#include "plate.hpp"
#include <cstdint>
#include <memory>

Plate::Plate(int w, int h) : width(w), height(h), preview_open(false) {
    for(int i=0;i<2;++i)
        for(int j=0;j<4;++j)
            mpfr_init_set_d(projection[i][j], (i==j?1.0:0.0), MPFR_RNDN);
    data.resize(width, std::vector<int64_t>(height, 0));
    row_mutexes.resize(height);
    for(int i=0;i<height;++i)
        row_mutexes[i].reset(new std::mutex());
}

Plate::~Plate() {
    for(int i=0;i<2;++i)
        for(int j=0;j<4;++j)
            mpfr_clear(projection[i][j]);
}

void Plate::insertPhoton(const Quaternion& q, int energy) {
    mpfr_t u,v,tmp;
    mpfr_inits(u,v,tmp,(mpfr_ptr)0);
    mpfr_set_d(u,0.0,MPFR_RNDN);
    mpfr_set_d(v,0.0,MPFR_RNDN);
    for(int j=0;j<4;++j){
        mpfr_mul(tmp, projection[0][j], q.component(j), MPFR_RNDN);
        mpfr_add(u,u,tmp,MPFR_RNDN);
        mpfr_mul(tmp, projection[1][j], q.component(j), MPFR_RNDN);
        mpfr_add(v,v,tmp,MPFR_RNDN);
    }
    double ud = mpfr_get_d(u, MPFR_RNDN);
    double vd = mpfr_get_d(v, MPFR_RNDN);
    mpfr_clears(u,v,tmp,(mpfr_ptr)0);

    int x = static_cast<int>((ud + 1.0) * 0.5 * width);
    int y = static_cast<int>((vd + 1.0) * 0.5 * height);
    if(x < 0 || x >= width || y < 0 || y >= height) return;
    std::lock_guard<std::mutex> lock(*row_mutexes[y]);
    data[x][y] += energy;
}
