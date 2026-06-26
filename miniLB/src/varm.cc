#include <varm.hh>
#include <file_manager.hh>
namespace bgk {

static constexpr char format_1003[sizeof("{:5d} {:14.6e} {:14.6e} {:14.6e}\n")] = "{:5d} {:14.6e} {:14.6e} {:14.6e}\n";
static constexpr char format_1005[sizeof("# t={:7d}\n")] = "# t={:7d}\n";

void varm(storage &bgk_storage, const int itime) {
    //To simplify indexing, those vectors will have one more element than the actual size
    auto u = std::make_unique<real_kinds::mykind[]>(bgk_storage.m + 1); // Mean velocity profiles
    auto w = std::make_unique<real_kinds::mykind[]>(bgk_storage.m + 1);
    auto den = std::make_unique<real_kinds::mykind[]>(bgk_storage.m + 1); // mean density profile
    real_kinds::mykind rho, rhoinv, rvol;
    real_kinds::mykind cte1;

#ifdef NOSHIFT
    cte1 = storage::zero;
#else
    cte1 = storage::uno;
#endif

    for(int j = 1; j <= bgk_storage.m; ++j) {
        u[j] = storage::zero;
        w[j] = storage::zero;
        den[j] = storage::zero;
    }

    for(int j = 1; j <= bgk_storage.m; ++j) {
        for(int i = 1; i <= bgk_storage.l; ++i) {
            rho = (bgk_storage.a01_host(i, j) + bgk_storage.a03_host(i, j) + bgk_storage.a05_host(i, j) + bgk_storage.a08_host(i, j)
                      + bgk_storage.a10_host(i, j) + bgk_storage.a12_host(i, j) + bgk_storage.a14_host(i, j) + bgk_storage.a17_host(i, j)
                      + bgk_storage.a19_host(i, j))
                + cte1;

            rhoinv = storage::uno / rho;

            den[j] += rho;

            u[j] += (bgk_storage.a01_host(i, j) + bgk_storage.a03_host(i, j) + bgk_storage.a05_host(i, j) - bgk_storage.a10_host(i, j)
                        - bgk_storage.a12_host(i, j) - bgk_storage.a14_host(i, j))
                * rhoinv;
            w[j] += (bgk_storage.a03_host(i, j) + bgk_storage.a08_host(i, j) + bgk_storage.a12_host(i, j) - bgk_storage.a01_host(i, j)
                        - bgk_storage.a10_host(i, j) - bgk_storage.a17_host(i, j))
                * rhoinv;
        }
    }

    rvol = 1.0 / static_cast<float>(bgk_storage.l);

    auto& file_manager = debug::file_manager::instance();
    file_manager.write_format<format_1005>(62, itime);
    for(int j = 1; j <= bgk_storage.m; ++j) {
        file_manager.write_format<format_1003>(62,  j+bgk_storage.offset[1], 
        u[j]*rvol,
        w[j]*rvol,
        den[j]*rvol);
    }
    file_manager.write(62, "\n\n"); //TODO: CHeck if this works

#ifdef DEBUG_1
    if(bgk_storage.myrank == 0) { std::cerr << "DEBUG1: Exiting from sub. varm" << "\n"; }
#endif
#ifdef MEM_CHECK
    if(bgk_storage.myrank == 0) {
        // Assuming get_mem() is a function that returns memory usage
        auto& timing = utils::timing::instance();
        timing.mem_stop = get_mem();
        std::cerr << "MEM_CHECK: after sub. varm mem = " << timing.mem_stop << "\n";
    }
#endif
}
} // namespace bgk
