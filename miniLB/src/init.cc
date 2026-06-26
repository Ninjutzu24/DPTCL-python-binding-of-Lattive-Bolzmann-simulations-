#include <init.hh>
#include <iostream>
#include <precision.hh>

namespace bgk {

void init(storage &bgk_storage, const int opt) {
#ifdef HALF_P
    real_kinds::sp xj, yj, x, y;
    real_kinds::sp cvsq, crho;
    real_kinds::sp cx01, cx03, cx05;
    real_kinds::sp cx08, cx10, cx12;
    real_kinds::sp cx14, cx17;
    real_kinds::sp cx19;
    real_kinds::sp cte1;

    constexpr real_kinds::sp pi = 3.141592653589793238462643383279;
    constexpr real_kinds::sp kappa = 80;
    constexpr real_kinds::sp delta = 0.05;
#else
    real_kinds::mykind xj, yj, x, y;
    real_kinds::mykind cvsq, crho;
    real_kinds::mykind cx01, cx03, cx05;
    real_kinds::mykind cx08, cx10, cx12;
    real_kinds::mykind cx14, cx17;
    real_kinds::mykind cx19;
    real_kinds::mykind cte1;
    
    constexpr real_kinds::mykind pi = 3.141592653589793238462643383279;
    constexpr real_kinds::mykind kappa = 80;
    constexpr real_kinds::mykind delta = 0.05;
#endif

    int ll, mm;

    // check parameter opt
    if(opt < 0 || opt > 5) {
        std::cerr << "Initial condition out of range[0,5] " << opt << "\n";
        std::exit(EXIT_FAILURE);
    }

#ifdef NOSHIFT
    cte1 = storage::uno;
#else
    cte1 = storage::zero;
#endif

    ll = bgk_storage.l;
    mm = bgk_storage.m;

    // builds the populations
    crho = real_kinds::mykind{1.0};

    for(int j = 0; j <= bgk_storage.m1; ++j) {
        y = (static_cast<real_kinds::mykind>(j) - 0.5) / static_cast<real_kinds::mykind>(mm); // 0<x<1 (taylor)
        // std::cout << j << ", " << y << "\n";
    }
    // std::cout << " " << "\n";
    for(int i = 0; i <= bgk_storage.l1; ++i) {
        x = (static_cast<real_kinds::mykind>(i) - 0.5) / static_cast<real_kinds::mykind>(ll); // 0<x<1 (taylor)
        // std::cout << i << ", " << x << "\n";
    }

    // rest flow
    xj = 0.0;
    yj = 0.0;

    for(int j = 0; j <= bgk_storage.m1; ++j) {
#ifdef PERIODIC
        y = (static_cast<real_kinds::mykind>(j) - 0.5) / static_cast<real_kinds::mykind>(mm); // 0<x<1 (taylor)
#endif
        for(int i = 0; i <= bgk_storage.l1; ++i) {
#ifdef PERIODIC
            x = (static_cast<real_kinds::mykind>(i) - 0.5) / static_cast<real_kinds::mykind>(ll); // 0<x<1 (taylor)

            // kida(?) vortices
            xj = 0.1 * std::sin(real_kinds::mykind{2.0} * pi * x) * std::cos(real_kinds::mykind{2.0} * pi * y);
            yj = -0.1 * std::cos(real_kinds::mykind{2.0} * pi * x) * std::sin(real_kinds::mykind{2.0} * pi * y);
#endif

            cvsq = xj * xj + yj * yj;

            cx01 = storage::rf * (xj - yj) + storage::qf * (3.0 * (xj - yj) * (xj - yj) - cvsq);
            cx03 = storage::rf * (xj + yj) + storage::qf * (3.0 * (xj + yj) * (xj + yj) - cvsq);
            cx05 = storage::rf * xj + storage::qf * (3.0 * xj * xj - cvsq);
            cx08 = storage::rf * yj + storage::qf * (3.0 * yj * yj - cvsq);
            cx10 = storage::rf * (-xj - yj) + storage::qf * (3.0 * (xj + yj) * (xj + yj) - cvsq);
            cx12 = storage::rf * (-xj + yj) + storage::qf * (3.0 * (xj - yj) * (xj - yj) - cvsq);
            cx14 = storage::rf * (-xj) + storage::qf * (3.0 * xj * xj - cvsq);
            cx17 = storage::rf * (-yj) + storage::qf * (3.0 * yj * yj - cvsq);
            cx19 = storage::rf * 0.0 + storage::qf * (3.0 * 0.0 * 0.0 - cvsq);

            bgk_storage.a01_host(i,j) = crho * storage::p2 * (cte1 + cx01);
            bgk_storage.a03_host(i,j) = crho * storage::p2 * (cte1 + cx03);
            bgk_storage.a05_host(i,j) = crho * storage::p1 * (cte1 + cx05);
            bgk_storage.a08_host(i,j) = crho * storage::p1 * (cte1 + cx08);
            bgk_storage.a10_host(i,j) = crho * storage::p2 * (cte1 + cx10);
            bgk_storage.a12_host(i,j) = crho * storage::p2 * (cte1 + cx12);
            bgk_storage.a14_host(i,j) = crho * storage::p1 * (cte1 + cx14);
            bgk_storage.a17_host(i,j) = crho * storage::p1 * (cte1 + cx17);
            bgk_storage.a19_host(i,j) = crho * storage::p0 * (cte1 + cx19);
        }
    }

#ifdef DEBUG_1
    if(bgk_storage.myrank == 0) { std::cout << "DEBUG1: Exiting from sub. init" << "\n"; }
#endif
}

} // namespace bgk