#include <vtk_om.hh>

#include "fmt/format.h"
#include <fstream>
#include <file_manager.hh>

namespace bgk {
void vtk_om(storage &bgk_storage, const int itime) {
    std::string file_name = "tec_om.xxxxxxxx.vtk";

    bgk_storage.myrank = 0;

    file_name.replace(7, 8, fmt::format("{:08}", itime));
    std::ofstream file(file_name);

    file << "# vtk DataFile Version 2.0\n";
    file << "Campo\n";
    file << "ASCII\n";
    file << "DATASET RECTILINEAR_GRID\n";
    file << fmt::format("DIMENSIONS {} {} {}\n", bgk_storage.l - 2, bgk_storage.m - 2, 1);

    file << fmt::format("X_COORDINATES {} double\n", bgk_storage.l - 2);
    for(int i = 2; i <= bgk_storage.l - 1; ++i) { file << i + bgk_storage.offset[0] << "\n"; }

    file << fmt::format("Y_COORDINATES {} double\n", bgk_storage.m - 2);
    for(int j = 2; j <= bgk_storage.m - 1; ++j) { file << j + bgk_storage.offset[1] << "\n"; }

    file << fmt::format("Z_COORDINATES {} double\n", 1);
    file << 0 << "\n";

    file << fmt::format("POINT_DATA {}\n", (bgk_storage.l - 2) * (bgk_storage.m - 2));
    file << "SCALARS vorticity double\n";
    file << "LOOKUP_TABLE default\n";

    real_kinds::sp up1, down1, left1, right1;
    real_kinds::sp d_up, d_down, d_left, d_right;
    real_kinds::sp vorticity;

    for(int j = 2; j <= bgk_storage.m - 1; ++j) {
        for(int i = 2; i <= bgk_storage.l - 1; ++i) {
            d_up = bgk_storage.a01_host(i,j + 1) + bgk_storage.a03_host(i,j + 1) + bgk_storage.a05_host(i,j + 1) + bgk_storage.a08_host(i,j + 1) + bgk_storage.a10_host(i,j + 1) + bgk_storage.a12_host(i,j + 1)
                + bgk_storage.a14_host(i,j + 1) + bgk_storage.a17_host(i,j + 1) + bgk_storage.a19_host(i,j + 1);

            d_down = bgk_storage.a01_host(i,j - 1) + bgk_storage.a03_host(i,j - 1) + bgk_storage.a05_host(i,j - 1) + bgk_storage.a08_host(i,j - 1) + bgk_storage.a10_host(i,j - 1) + bgk_storage.a12_host(i,j - 1)
                + bgk_storage.a14_host(i,j - 1) + bgk_storage.a17_host(i,j - 1) + bgk_storage.a19_host(i,j - 1);

            d_left = bgk_storage.a01_host(i - 1,j) + bgk_storage.a03_host(i - 1,j) + bgk_storage.a05_host(i - 1,j) + bgk_storage.a08_host(i - 1,j) + bgk_storage.a10_host(i - 1,j) + bgk_storage.a12_host(i - 1,j)
                + bgk_storage.a14_host(i - 1,j) + bgk_storage.a17_host(i - 1,j) + bgk_storage.a19_host(i - 1,j);

            d_right = bgk_storage.a01_host(i + 1,j) + bgk_storage.a03_host(i + 1,j) + bgk_storage.a05_host(i + 1,j) + bgk_storage.a08_host(i + 1,j) + bgk_storage.a10_host(i + 1,j)
                + bgk_storage.a12_host(i + 1,j) + bgk_storage.a14_host(i + 1,j) + bgk_storage.a17_host(i + 1,j) + bgk_storage.a19_host(i + 1,j);

            up1 = (bgk_storage.a01_host(i,j + 1) + bgk_storage.a03_host(i,j + 1) + bgk_storage.a05_host(i,j + 1) - bgk_storage.a10_host(i,j + 1) - bgk_storage.a12_host(i,j + 1) - bgk_storage.a14_host(i,j + 1))
                / d_up;

            down1
                = (bgk_storage.a01_host(i,j - 1) + bgk_storage.a03_host(i,j - 1) + bgk_storage.a05_host(i,j - 1) - bgk_storage.a10_host(i,j - 1) - bgk_storage.a12_host(i,j - 1) - bgk_storage.a14_host(i,j - 1))
                / d_down;

            left1
                = (bgk_storage.a03_host(i - 1,j) + bgk_storage.a08_host(i - 1,j) + bgk_storage.a12_host(i - 1,j) - bgk_storage.a01_host(i - 1,j) - bgk_storage.a10_host(i - 1,j) - bgk_storage.a17_host(i - 1,j))
                / d_left;

            right1
                = (bgk_storage.a03_host(i + 1,j) + bgk_storage.a08_host(i + 1,j) + bgk_storage.a12_host(i + 1,j) - bgk_storage.a01_host(i + 1,j) - bgk_storage.a10_host(i + 1,j) - bgk_storage.a17_host(i + 1,j))
                / d_right;

            vorticity = 0.5 * (up1 - down1) - 0.5 * (right1 - left1);
            file << fmt::format("{:14.6e} ", vorticity) << "\n";
        }
    }

    file.close();

    std::cout << "I/O : vorticity (vtk) done" << "\n";
    auto& file_manager = bgk::debug::file_manager::instance();
    file_manager.write(16, "I/O : vorticity (vtk) done\n");

#ifdef DEBUG_1
    if(bgk_storage.myrank == 0) { std::cout << "DEBUG1: Exiting from sub. vtk_om" << "\n"; }
#endif
}
} // namespace bgk