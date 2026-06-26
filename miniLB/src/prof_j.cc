#include <file_manager.hh>
#include <prof_j.hh>

#include <sycl/sycl.hpp>

namespace bgk {

static constexpr char format_1005[sizeof("# t={:7d}\n")] = "# t={:7d}\n";
static inline constexpr char format_1002[sizeof("{:14.6e} {:14.6e} {:14.6e} {:14.6e}\n")]
    = "{:14.6e} {:14.6e} {:14.6e} {:14.6e}\n";

void prof_j(storage &bgk_storage, const int itime, const int icoord) {
    // Variables
    std::vector<real_kinds::mykind> u(bgk_storage.m + 1);
    std::vector<real_kinds::mykind> v(bgk_storage.m + 1);
    std::vector<real_kinds::mykind> den(bgk_storage.m + 1);
    real_kinds::mykind cte1;

    // Set cte1 based on NOSHIFT
#ifdef NOSHIFT
    cte1 = storage::zero;
#else
    cte1 = storage::uno;
#endif

    auto &q = *bgk_storage.host_q;
    // for (int j = 0; j < bgk_storage.m; ++j) {
    //     den[j] = (bgk_storage.a01_host(icoord,j) + bgk_storage.a03_host(icoord,j) + bgk_storage.a05_host(icoord,j) +
    //               bgk_storage.a08_host(icoord,j) + bgk_storage.a10_host(icoord,j) + bgk_storage.a12_host(icoord,j) +
    //               bgk_storage.a14_host(icoord,j) + bgk_storage.a17_host(icoord,j) + bgk_storage.a19_host(icoord,j)) +
    //               cte1;
    // }

    // Density calculation
    auto event = q.parallel_for(sycl::range<1>{static_cast<size_t>(bgk_storage.m)},
        [den = den.data(), icoord, cte1, a01 = bgk_storage.a01_host, a03 = bgk_storage.a03_host,
            a05 = bgk_storage.a05_host, a08 = bgk_storage.a08_host, a10 = bgk_storage.a10_host,
            a12 = bgk_storage.a12_host, a14 = bgk_storage.a14_host, a17 = bgk_storage.a17_host,
            a19 = bgk_storage.a19_host](sycl::item<1> idx) {
            const auto j = idx.get_linear_id() + 1;
            den[j] = (a01(icoord, j) + a03(icoord, j) + a05(icoord, j) + a08(icoord, j) + a10(icoord, j)
                         + a12(icoord, j) + a14(icoord, j) + a17(icoord, j) + a19(icoord, j))
                + cte1;
        });


    // Streamwise velocity calculation
    // for (int j = 0; j < bgk_storage.m; ++j) {
    //     u[j] = (bgk_storage.a01_host(icoord,j) - bgk_storage.a10_host(icoord,j) +
    //             bgk_storage.a03_host(icoord,j) - bgk_storage.a12_host(icoord,j) +
    //             bgk_storage.a05_host(icoord,j) - bgk_storage.a14_host(icoord,j)) / den[j];
    // }
    event = q.parallel_for(sycl::range<1>{static_cast<size_t>(bgk_storage.m)}, event,
        [u = u.data(), icoord, den = den.data(), a01 = bgk_storage.a01_host, a03 = bgk_storage.a03_host,
            a05 = bgk_storage.a05_host, a10 = bgk_storage.a10_host, a12 = bgk_storage.a12_host,
            a14 = bgk_storage.a14_host](sycl::item<1> idx) {
            const auto j = idx.get_linear_id() + 1;
            u[j] = (a01(icoord, j) - a10(icoord, j) + a03(icoord, j) - a12(icoord, j) + a05(icoord, j) - a14(icoord, j))
                / den[j];
        });


    // Normal-to-wall velocity calculation
    // for (int j = 0; j < bgk_storage.m; ++j) {
    //     v[j] = (bgk_storage.a03_host(icoord,j) - bgk_storage.a01_host(icoord,j) +
    //             bgk_storage.a08_host(icoord,j) - bgk_storage.a17_host(icoord,j) +
    //             bgk_storage.a12_host(icoord,j) - bgk_storage.a10_host(icoord,j)) / den[j];
    // }
    q.parallel_for(sycl::range<1>{static_cast<size_t>(bgk_storage.m)}, event,
         [v = v.data(), icoord, den = den.data(), a01 = bgk_storage.a01_host, a03 = bgk_storage.a03_host,
             a08 = bgk_storage.a08_host, a10 = bgk_storage.a10_host, a12 = bgk_storage.a12_host,
             a17 = bgk_storage.a17_host](sycl::item<1> idx) {
             const auto j = idx.get_linear_id() + 1;
             v[j]
                 = (a03(icoord, j) - a01(icoord, j) + a08(icoord, j) - a17(icoord, j) + a12(icoord, j) - a10(icoord, j))
                 / den[j];
         })
        .wait_and_throw();

    // Write itime
    auto &file_manager = debug::file_manager::instance();
    file_manager.write_format<format_1005>(64, itime);
    // Write data
    for(int j = 1; j <= bgk_storage.m; ++j) {
        file_manager.write_format<format_1002>(64, (j - 0.5), u[j], v[j], den[j]);
    }

    // Write empty lines
    file_manager.write(64, "\n\n");

    // Debug output
#ifdef DEBUG_1
    if(bgk_storage.myrank == 0) { std::cout << "DEBUG1: Exiting from sub. prof_j" << "\n"; }
#endif
}

} // namespace bgk