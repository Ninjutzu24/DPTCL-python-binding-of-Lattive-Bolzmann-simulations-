#include <draglift.hh>
#include <file_manager.hh>
#include <fmt/core.h>
#include <time.hh>

namespace bgk {
void draglift(storage &bgk_storage, const int itime) {
    int i;
    int j;

    // cylinder center
    int icoord = static_cast<int>(2.0 * bgk_storage.l / 5.0);
    int jcoord = static_cast<int>(bgk_storage.m / 2.0);

    // border
    int delta = 3;

    // bounding box
    int istart = icoord - bgk_storage.radius - delta;
    int istop = icoord +  bgk_storage.radius + delta;
    int jstart = jcoord - bgk_storage.radius - delta;
    int jstop = jcoord + bgk_storage.radius + delta;

    // computing drag (force along x)
    real_kinds::mykind forceX = 0.0;
    real_kinds::mykind force01;
    real_kinds::mykind force03;
    real_kinds::mykind force05;
    real_kinds::mykind force08;
    real_kinds::mykind force10;
    real_kinds::mykind force12;
    real_kinds::mykind force14;
    real_kinds::mykind force17;

    for(j = jstart; j <= jstop; ++j) {
        for(i = istart; i <= istop; ++i) {
            if(bgk_storage.obs_host(i,j) == 0) {
                force01 = 2.0 * storage::cx[1] * bgk_storage.a01_host(i,j) * bgk_storage.obs_host(i + storage::icx[1],j + storage::icy[1]);
                force03 = 2.0 * storage::cx[3] * bgk_storage.a03_host(i,j) * bgk_storage.obs_host(i + storage::icx[3],j + storage::icy[3]);
                force05 = 2.0 * storage::cx[5] * bgk_storage.a05_host(i,j) * bgk_storage.obs_host(i + storage::icx[5],j + storage::icy[5]);
                force08 = 2.0 * storage::cx[8] * bgk_storage.a08_host(i,j) * bgk_storage.obs_host(i + storage::icx[8],j + storage::icy[8]);
                force10 = 2.0 * storage::cx[10] * bgk_storage.a10_host(i,j) * bgk_storage.obs_host(i + storage::icx[10],j + storage::icy[10]);
                force12 = 2.0 * storage::cx[12] * bgk_storage.a12_host(i,j) * bgk_storage.obs_host(i + storage::icx[12],j + storage::icy[12]);
                force14 = 2.0 * storage::cx[14] * bgk_storage.a14_host(i,j) * bgk_storage.obs_host(i + storage::icx[14],j + storage::icy[14]);
                force17 = 2.0 * storage::cx[17] * bgk_storage.a17_host(i,j) * bgk_storage.obs_host(i + storage::icx[17],j + storage::icy[17]);

                forceX += (force01 + force03 + force05 + force08 + force10 + force12 + force14 + force17);
            }
        }
    }

    // computing lift (force along y)
    real_kinds::mykind forceY = 0.0;
    for(j = jstart; j <= jstop; ++j) {
        for(i = istart; i <= istop; ++i) {
            if(bgk_storage.obs_host(i,j) == 0) {
                force01 = 2.0 * storage::cy[1] * bgk_storage.a01_host(i,j) * bgk_storage.obs_host(i + storage::icx[1],j + storage::icy[1]);
                force03 = 2.0 * storage::cy[3] * bgk_storage.a03_host(i,j) * bgk_storage.obs_host(i + storage::icx[3],j + storage::icy[3]);
                force05 = 2.0 * storage::cy[5] * bgk_storage.a05_host(i,j) * bgk_storage.obs_host(i + storage::icx[5],j + storage::icy[5]);
                force08 = 2.0 * storage::cy[8] * bgk_storage.a08_host(i,j) * bgk_storage.obs_host(i + storage::icx[8],j + storage::icy[8]);
                force10 = 2.0 * storage::cy[10] * bgk_storage.a10_host(i,j) * bgk_storage.obs_host(i + storage::icx[10],j + storage::icy[10]);
                force12 = 2.0 * storage::cy[12] * bgk_storage.a12_host(i,j) * bgk_storage.obs_host(i + storage::icx[12],j + storage::icy[12]);
                force14 = 2.0 * storage::cy[14] * bgk_storage.a14_host(i,j) * bgk_storage.obs_host(i + storage::icx[14],j + storage::icy[14]);
                force17 = 2.0 * storage::cy[17] * bgk_storage.a17_host(i,j) * bgk_storage.obs_host(i + storage::icx[17],j + storage::icy[17]);

                forceY += (force01 + force03 + force05 + force08 + force10 + force12 + force14 + force17);
            }
        }
    }

    real_kinds::mykind norm = 1.0 / (bgk_storage.u_inflow * bgk_storage.u_inflow * bgk_storage.radius);
    auto& file_manager = debug::file_manager::instance();
    file_manager.write_format<"{} {} {}\n">(66, itime, forceX * norm, forceY * norm);

#ifdef DEBUG_2
    if(myrank == 0) {
        fmt::print("DEBUG2: Exiting from sub. drag2 {}\n", itime);
        fmt::print("DEBUG2 {} {} {}\n", forceX, forceY, norm);
    }
#endif
}
} // namespace bgk