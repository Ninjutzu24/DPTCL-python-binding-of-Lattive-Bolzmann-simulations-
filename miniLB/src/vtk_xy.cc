#include <vtk_xy.hh>
#include <fmt/format.h>
#include <iostream>
#include <fstream>
#include <file_manager.hh>

namespace bgk{
    static constexpr auto format_1004 = "{:14.6e} {:14.6e} {:14.6e}\n";
    static constexpr auto format_1005 = "{:14.6e}\n";
    static constexpr auto format_4000 = "{:08d}";

    void vtk_xy(storage &bgk_storage, const int itime){
        // Variables
    std::string file_name = "tec_xy.xxxxxxxx.vtk";
    real_kinds::sp u;
    real_kinds::sp w;
    real_kinds::sp den;
    real_kinds::sp cte1;

    // Set cte1 based on NOSHIFT
#ifdef NOSHIFT
    cte1 = storage::zero;
#else
    cte1 = storage::uno;
#endif

    // Update file name with itime
    file_name.replace(7, 8, fmt::format(format_4000, itime));

    // Open file
    std::ofstream file(file_name);

    // Write headers
    file << "# vtk DataFile Version 2.0\n";
    file << "Campo\n";
    file << "ASCII\n";
    file << "DATASET RECTILINEAR_GRID\n";
    file << fmt::format("DIMENSIONS {:d} {:d} {:d}\n", bgk_storage.l, bgk_storage.m, 1);

    // Write coordinates
    file << fmt::format("X_COORDINATES {:d} double\n", bgk_storage.l);
    // for (int i = 1; i <= l; ++i) {
    //     file << fmt::format("{} ", i + offset[0]);
    // }
    // file << "\n";

    file << fmt::format("Y_COORDINATES {:d} double\n", bgk_storage.m);
    // for (int j = 1; j <= m; ++j) {
    //     file << fmt::format("{} ", j + offset[1]);
    // }
    // file << "\n";

    file << fmt::format("Z_COORDINATES {:d} double\n", 1);
    file << "0\n";

    // Write point data
    file << fmt::format("POINT_DATA {:d}\n", bgk_storage.l * bgk_storage.m * 1);
    file << "VECTORS velocity double\n";
    // file << "LOOKUP_TABLE default\n";

    for (int j = 1; j <= bgk_storage.m; ++j) {
        for (int i = 1; i <= bgk_storage.l; ++i) {
            u = bgk_storage.a01_host(i,j) + bgk_storage.a03_host(i,j) + bgk_storage.a05_host(i,j) - bgk_storage.a10_host(i,j) - bgk_storage.a12_host(i,j) - bgk_storage.a14_host(i,j);
            w = bgk_storage.a03_host(i,j) + bgk_storage.a08_host(i,j) + bgk_storage.a12_host(i,j) - bgk_storage.a01_host(i,j) - bgk_storage.a10_host(i,j) - bgk_storage.a17_host(i,j);
            den = bgk_storage.a01_host(i,j) + bgk_storage.a03_host(i,j) + bgk_storage.a05_host(i,j) + bgk_storage.a08_host(i,j) + bgk_storage.a10_host(i,j) + bgk_storage.a12_host(i,j) + bgk_storage.a14_host(i,j) + bgk_storage.a17_host(i,j) + bgk_storage.a19_host(i,j) + cte1;
            file << fmt::format(format_1004, u / den, w / den, 0.0);
            std::cout << i << " " << j << " " << u << " " << w << " " << den << "\n";
        }
    }

    // Write scalar data
    file << "SCALARS u double\n";
    file << "LOOKUP_TABLE default\n";
    for (int j = 1; j <= bgk_storage.m; ++j) {
        for (int i = 1; i <= bgk_storage.l; ++i) {
            u = bgk_storage.a01_host(i,j) + bgk_storage.a03_host(i,j) + bgk_storage.a05_host(i,j) - bgk_storage.a10_host(i,j) - bgk_storage.a12_host(i,j) - bgk_storage.a14_host(i,j);
            den = bgk_storage.a01_host(i,j) + bgk_storage.a03_host(i,j) + bgk_storage.a05_host(i,j) + bgk_storage.a08_host(i,j) + bgk_storage.a10_host(i,j) + bgk_storage.a12_host(i,j) + bgk_storage.a14_host(i,j) + bgk_storage.a17_host(i,j) + bgk_storage.a19_host(i,j) + cte1;
            file << fmt::format(format_1005, u / den);
        }
    }

    file << "SCALARS w double\n";
    file << "LOOKUP_TABLE default\n";
    for (int j = 1; j <= bgk_storage.m; ++j) {
        for (int i = 1; i <= bgk_storage.l; ++i) {
            w = bgk_storage.a03_host(i,j) + bgk_storage.a08_host(i,j) + bgk_storage.a12_host(i,j) - bgk_storage.a01_host(i,j) - bgk_storage.a10_host(i,j) - bgk_storage.a17_host(i,j);
            den = bgk_storage.a01_host(i,j) + bgk_storage.a03_host(i,j) + bgk_storage.a05_host(i,j) + bgk_storage.a08_host(i,j) + bgk_storage.a10_host(i,j) + bgk_storage.a12_host(i,j) + bgk_storage.a14_host(i,j) + bgk_storage.a17_host(i,j) + bgk_storage.a19_host(i,j) + cte1;
            file << fmt::format(format_1005, w / den);
        }
    }

    file << "SCALARS rho double\n";
    file << "LOOKUP_TABLE default\n";
    for (int j = 1; j <= bgk_storage.m; ++j) {
        for (int i = 1; i <= bgk_storage.l; ++i) {
            den = bgk_storage.a01_host(i,j) + bgk_storage.a03_host(i,j) + bgk_storage.a05_host(i,j) + bgk_storage.a08_host(i,j) + bgk_storage.a10_host(i,j) + bgk_storage.a12_host(i,j) + bgk_storage.a14_host(i,j) + bgk_storage.a17_host(i,j) + bgk_storage.a19_host(i,j) + cte1;
            file << fmt::format(format_1005, den);
        }
    }

    // Close file
    file.close();
    std::cout << "I/O : plane xy (vtk) done\n";
    auto& file_manager = debug::file_manager::instance();
    file_manager.write(16, "I/O : plane xy (vtk) done\n");

#ifdef DEBUG_1
    if (bgk_storage.myrank == 0) {
        std::cout << "DEBUG1: Exiting from sub. vtk_xy\n";
    }
#endif
    }
}