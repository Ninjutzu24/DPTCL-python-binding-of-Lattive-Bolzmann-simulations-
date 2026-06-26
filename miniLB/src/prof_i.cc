#include <file_manager.hh>
#include <prof_i.hh>

#include <sycl/sycl.hpp>

namespace bgk {

static inline constexpr char format_1002[sizeof("{:14.6e} {:14.6e} {:14.6e} {:14.6e}\n")]
    = "{:14.6e} {:14.6e} {:14.6e} {:14.6e}\n";
static inline constexpr char format_1005[sizeof("# t={:7d}\n")] = "# t={:7d}\n";

void prof_i(storage &bgk_storage, const int itime, const int jcoord) {
    std::vector<real_kinds::mykind> u(bgk_storage.l + 1);
    std::vector<real_kinds::mykind> v(bgk_storage.l + 1);
    std::vector<real_kinds::mykind> den(bgk_storage.l + 1);
    real_kinds::mykind cte1;

// Set cte1 based on NOSHIFT
#ifdef NOSHIFT
    cte1 = storage::zero;
#else
    cte1 = storage::uno;
#endif

    // Density calculation
    bgk_storage.dev_q->wait();
    auto &q = *bgk_storage.host_q;
    
    // for(int i = 1; i <= bgk_storage.l; ++i) {
    //     den[i] = (bgk_storage.a01_host(i,jcoord) + bgk_storage.a03_host(i,jcoord) + bgk_storage.a05_host(i,jcoord) +
    //     bgk_storage.a08_host(i,jcoord) + bgk_storage.a10_host(i,jcoord) + bgk_storage.a12_host(i,jcoord)
    //                  + bgk_storage.a14_host(i,jcoord) + bgk_storage.a17_host(i,jcoord) + bgk_storage.a19_host(i,jcoord))
    //         + cte1;
    // }

    auto event = q.parallel_for(sycl::range<1>{static_cast<size_t>(bgk_storage.l)},
        [den = den.data(), jcoord, cte1, a01 = bgk_storage.a01_host, a03 = bgk_storage.a03_host,
            a05 = bgk_storage.a05_host, a08 = bgk_storage.a08_host, a10 = bgk_storage.a10_host,
            a12 = bgk_storage.a12_host, a14 = bgk_storage.a14_host, a17 = bgk_storage.a17_host,
            a19 = bgk_storage.a19_host](sycl::item<1> idx) {
            const auto i = idx.get_linear_id() + 1;
            den[i] = (a01(i, jcoord) + a03(i, jcoord) + a05(i, jcoord) + a08(i, jcoord) + a10(i, jcoord)
                         + a12(i, jcoord) + a14(i, jcoord) + a17(i, jcoord) + a19(i, jcoord))
                + cte1;
        });


    // Streamwise velocity calculation
    // for(int i = 1; i <= bgk_storage.l; ++i) {
    //     u[i] = (bgk_storage.a01_host(i,jcoord) + bgk_storage.a03_host(i,jcoord) + bgk_storage.a05_host(i,jcoord) -
    //     bgk_storage.a10_host(i,jcoord) - bgk_storage.a12_host(i,jcoord) - bgk_storage.a14_host(i,jcoord))
    //         / den[i];
    // }
//     // Streamwise velocity calculation
    [[maybe_unused]] auto event2 = q.submit([&](sycl::handler &cgh) {
#ifndef SYCL_IN_ORDER_QUEUE
        cgh.depends_on(event);
#endif
        cgh.parallel_for(sycl::range<1>{static_cast<size_t>(bgk_storage.l)},
            [u = u.data(), jcoord, den = den.data(), a01 = bgk_storage.a01_host, a03 = bgk_storage.a03_host,
                a05 = bgk_storage.a05_host, a10 = bgk_storage.a10_host, a12 = bgk_storage.a12_host,
                a14 = bgk_storage.a14_host](sycl::item<1> idx) {
                const auto i = idx.get_linear_id() + 1;
                u[i] = (a01(i, jcoord) + a03(i, jcoord) + a05(i, jcoord) - a10(i, jcoord) - a12(i, jcoord)
                           - a14(i, jcoord))
                    / den[i];
            });
    });

    // Normal-to-wall velocity calculation
    // for(int i = 1; i <= bgk_storage.l; ++i) {
    //     v[i] = (bgk_storage.a03_host(i,jcoord) + bgk_storage.a08_host(i,jcoord) + bgk_storage.a12_host(i,jcoord) -
    //     bgk_storage.a01_host(i,jcoord) - bgk_storage.a10_host(i,jcoord) - bgk_storage.a17_host(i,jcoord))
    //         / den[i];
    // }

    [[maybe_unused]] auto event3 = q.submit([&](sycl::handler &cgh) {
#ifndef SYCL_IN_ORDER_QUEUE
        cgh.depends_on(event2);
#endif
        cgh.parallel_for(sycl::range<1>{static_cast<size_t>(bgk_storage.l)},
            [v = v.data(), jcoord, den = den.data(), a01 = bgk_storage.a01_host, a03 = bgk_storage.a03_host,
                a08 = bgk_storage.a08_host, a10 = bgk_storage.a10_host, a12 = bgk_storage.a12_host,
                a17 = bgk_storage.a17_host](sycl::item<1> idx) {
                const auto i = idx.get_linear_id() + 1;
                v[i] = (a03(i, jcoord) + a08(i, jcoord) + a12(i, jcoord) - a01(i, jcoord) - a10(i, jcoord)
                           - a17(i, jcoord))
                    / den[i];
            });
    });


    event3.wait();

            auto &file_manager = debug::file_manager::instance();
            file_manager.write_format<format_1005>(61, itime);
            // Write data
            for(int i = 1; i <= bgk_storage.l; ++i) {
                file_manager.write_format<format_1002>(61, (i - 0.5), u[i], v[i], den[i]);
            }
            // Write empty lines
            file_manager.write(61, "\n\n");

// Debug output
#ifdef DEBUG_1
    if(bgk_storage.myrank == 0) {
        std::cout << "DEBUG1: Exiting from sub. prof_i"
                  << "\n";
    }
#endif
}
} // namespace bgk
