#include <build_obs.hh>
#include <fmt/core.h>
#include <iostream>
#include <random>
#include <file_manager.hh>

namespace bgk {
void build_obs(storage &bgk_storage) {
    int i, j;
    int icoord, jcoord;
    int itime;
    real_kinds::mykind d2, R2a, R2b, R;
    real_kinds::mykind hsize;
    real_kinds::mykind soglia, a;
    real_kinds::mykind rand;
    bgk_storage.imin = bgk_storage.l;
    bgk_storage.jmin = bgk_storage.m;
    bgk_storage.imax = 0;
    bgk_storage.jmax = 0;
    bgk_storage.nobs = 0;
    bgk_storage.myrank = 0;
    itime = 0;
    
    auto& file_manager = debug::file_manager::instance();
    // Creating obstacle
#ifdef SQUARE
    // Creating square
    fmt::print("INFO: creating obstacle (square)\n");
    // Center of the square
    icoord = 2 * bgk_storage.l / 5;
    jcoord = bgk_storage.m / 2;
    hsize = bgk_storage.radius; // Half size of the square
    fmt::print("INFO: square size   --> {}, {}\n", 2 * hsize, 2 * hsize / bgk_storage.m);
    fmt::print("INFO: Cyl icoord    --> {}, {}\n", icoord, icoord / bgk_storage.l);
    fmt::print("INFO: Cyl jcoord    --> {}, {}\n", jcoord, jcoord / bgk_storage.m);
    for(int j = 1; j <= bgk_storage.m; ++j) {
        if((j >= (jcoord - hsize)) && (j <= (jcoord + hsize))) {
            for(int i = 1; i <= bgk_storage.l; ++i) {
                if((i >= (icoord - hsize)) && (i <= (icoord + hsize))) {
                    bgk_storage.obs(i, j) = 1;
                    bgk_storage.nobs += 1;
                    bgk_storage.imin = std::min(bgk_storage.imin, i);
                    bgk_storage.jmin = std::min(bgk_storage.jmin, j);
                    bgk_storage.imax = std::max(bgk_storage.imax, i);
                    bgk_storage.jmax = std::max(bgk_storage.jmax, j);
                }
            }
        }
    }
#elif defined(POROUS)
    fmt::print("INFO: creating obstacle (porous)\n");
    fmt::print("INFO: creating obstacle (porous)\n");
    // Threshold
    soglia = 0.4;
    fmt::print("INFO: threshold (porous) {}\n", soglia);
    fmt::print("INFO: threshold (porous) {}\n", soglia);
    for(int j = 5; j <= bgk_storage.m - 4; j += 5) {
        for(int i = bgk_storage.l / 4; i <= 3 * bgk_storage.l / 4; i += 5) {
            double a = static_cast<double>(std::rand()) / RAND_MAX;
            if(a > soglia) {
                bgk_storage.obs(i + 1, j) = 1;
                bgk_storage.obs(i, j + 1) = 1;
                bgk_storage.obs(i, j) = 1;
                bgk_storage.obs(i - 1, j) = 1;
                bgk_storage.obs(i, j - 1) = 1;
                bgk_storage.nobs += 5;
                bgk_storage.imin = std::min(bgk_storage.imin, i - 1);
                bgk_storage.jmin = std::min(bgk_storage.jmin, j - 1);
                bgk_storage.imax = std::max(bgk_storage.imax, i + 1);
                bgk_storage.jmax = std::max(bgk_storage.jmax, j + 1);
            }
        }
    }
#else
    fmt::print("INFO: creating obstacle (cylinder)\n");
    file_manager.write(16, "INFO: creating obstacle (cylinder)\n");
    icoord = 2 * bgk_storage.l / 5;
    jcoord = bgk_storage.m / 2;
    fmt::print("INFO: Cyl radius    --> {}, {}\n", bgk_storage.radius, bgk_storage.radius / bgk_storage.m);
    fmt::print("INFO: Cyl icoord    --> {}, {}\n", icoord, icoord / bgk_storage.l);
    fmt::print("INFO: Cyl jcoord    --> {}, {}\n", jcoord, jcoord / bgk_storage.m);
    R = bgk_storage.radius * bgk::storage::uno;
    R2a = (R - 2) * (R - 2); // lower radius
    R2b = (R + 2) * (R + 2); // upper radius
    for(int j = 1; j <= bgk_storage.m; ++j) {
        for(int i = 1; i <= bgk_storage.l; ++i) {
            double d2 = (icoord - i) * (icoord - i) + (jcoord - j) * (jcoord - j);
            if(d2 > R2a && d2 < R2b) {
                bgk_storage.obs_host(i, j) = 1;
                bgk_storage.nobs += 1;
                bgk_storage.imin = std::min(bgk_storage.imin, i);
                bgk_storage.jmin = std::min(bgk_storage.jmin, j);
                bgk_storage.imax = std::max(bgk_storage.imax, i);
                bgk_storage.jmax = std::max(bgk_storage.jmax, j);
            }
        }
    }
#endif
    fmt::print("INFO: num. obs         --> {}\n", bgk_storage.nobs);
    fmt::print(
        "INFO: ratio obs/size   --> {}\n", static_cast<float>(bgk_storage.nobs) / (bgk_storage.l * bgk_storage.m));
    fmt::print("INFO: obs (x)             {}, {}\n", bgk_storage.imax, bgk_storage.imin);
    fmt::print("INFO: obs (y)             {}, {}\n", bgk_storage.jmax, bgk_storage.jmin);
#ifdef DEBUG_1
    if(bgk_storage.myrank == 0) { fmt::print("DEBUG1: Exiting from sub. build_obs\n"); }
#endif
}
} // namespace bgk