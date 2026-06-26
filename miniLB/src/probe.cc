#include <probe.hh>
#include <file_manager.hh>

namespace bgk{
    void probe(storage& bgk_storage, const int itime, const int i0, const int j0){
        real_kinds::mykind rho, xj, yj;
        real_kinds::mykind x01, x03, x05, x08, x10;
        real_kinds::mykind x12, x14, x17, x19;
        real_kinds::mykind cte1;
        auto& file_manager = debug::file_manager::instance();

    #ifdef NOSHIFT
        cte1 = storage::zero;
    #else
        cte1 = storage::uno;
    #endif

        x01 = bgk_storage.a01_host(i0,j0);
        x03 = bgk_storage.a03_host(i0,j0);
        x05 = bgk_storage.a05_host(i0,j0);
        x08 = bgk_storage.a08_host(i0,j0);
        x10 = bgk_storage.a10_host(i0,j0);
        x12 = bgk_storage.a12_host(i0,j0);
        x14 = bgk_storage.a14_host(i0,j0);
        x17 = bgk_storage.a17_host(i0,j0);
        x19 = bgk_storage.a19_host(i0,j0);

        rho = (x01 + x03 + x05 + x08 + x10 + x12 + x14 + x17 + x19) + cte1;

        xj = (x01 + x03 + x05 - x10 - x12 - x14);
        yj = (x03 + x08 + x12 - x01 - x10 - x17);

        file_manager.write_format<" Timestep {:8} {:14.6e} {:14.6e} {:14.6e}\n">(68, itime, xj / rho, yj / rho, rho);

    #ifdef DEBUG_2
        if (bgk_storage.myrank == 0) {
            fmt::print("DEBUG2: Exiting from sub. probe {}, {}\n", i0, j0);
        }
#endif
    }
}