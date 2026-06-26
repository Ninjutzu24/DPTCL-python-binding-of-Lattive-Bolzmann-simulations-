#include <diagno.hh>
#include <file_manager.hh>

namespace bgk {
void diagno(storage &bgk_storage, const int itime) {
    real_kinds::mykind rtot{0}, xtot{0}, ytot{0}, stot{0};
    real_kinds::mykind xj, yj, rho, rhoinv, rdv;
    real_kinds::mykind x01, x03, x05, x08, x10;
    real_kinds::mykind x12, x14, x17, x19;
    real_kinds::mykind cte1;

#ifdef NOSHIFT
    cte1 = storage::zero;
#else
    cte1 = storage::uno;
#endif

    rdv = storage::uno / (static_cast<real_kinds::mykind>(bgk_storage.l) * static_cast<real_kinds::mykind>(bgk_storage.m));
    
    auto& file_manager = debug::file_manager::instance();

    //TODO Offload?
    for(int j = 1; j <= bgk_storage.m; ++j) {
        for(int i = 1; i <= bgk_storage.l; ++i) {
            x01 = bgk_storage.a01_host(i,j);
            x03 = bgk_storage.a03_host(i,j);
            x05 = bgk_storage.a05_host(i,j);
            x08 = bgk_storage.a08_host(i,j);
            x10 = bgk_storage.a10_host(i,j);
            x12 = bgk_storage.a12_host(i,j);
            x14 = bgk_storage.a14_host(i,j);
            x17 = bgk_storage.a17_host(i,j);
            x19 = bgk_storage.a19_host(i,j);

            rho = (((x01 + x03 + x10 + x12) + (x05 + x08 + x14 + x17)) + x19) + cte1;
            rhoinv = storage::uno / rho;

            xj = ((x03 - x12) + (x01 - x10) + (x05 - x14)) * rhoinv;
            yj = ((x03 - x01) + (x12 - x10) + (x08 - x17)) * rhoinv;
            
            // file_manager.write_format<"i = {:3}, j = {:3} => yj = {:14.6e}\n">(16,i, j, yj);
            
            rtot += rho;
            xtot += xj;
            ytot += yj;
            stot += (xj * xj + yj * yj);
        }
    }

    rtot = (rtot / static_cast<float>(bgk_storage.l)) / static_cast<float>(bgk_storage.m);
    xtot = (xtot / static_cast<float>(bgk_storage.l)) / static_cast<float>(bgk_storage.m);
    ytot = (ytot / static_cast<float>(bgk_storage.l)) / static_cast<float>(bgk_storage.m);
    stot = (stot / static_cast<float>(bgk_storage.l)) / static_cast<float>(bgk_storage.m);

    file_manager.write(16, fmt::format(" Timestep {:8}\n", itime));
    file_manager.write(16, fmt::format("       mean rho {:14.6e}\n", rtot));
    file_manager.write(16, fmt::format("       mean vel {:14.6e} {:14.6e} {:14.6e}\n", xtot, ytot, stot));
    file_manager.write(63, fmt::format("{:8} {:14.6e} {:14.6e} {:14.6e} {:14.6e}\n", itime, xtot, ytot, rtot, stot));

    file_manager.flush(16);
    file_manager.flush(63);

#ifdef DEBUG_1
    if(bgk_storage.myrank == 0) { fmt::print("DEBUG1: Exiting from sub. diagno\n"); }
#endif
}
} // namespace bgk