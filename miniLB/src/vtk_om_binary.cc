#include "fmt/format.h"
#include <file_manager.hh>
#include <fstream>
#include <vtk_om_binary.hh>

namespace bgk {
void vtk_om_bin(storage &bgk_storage, const int itime) {

    real_kinds::sp den, uv, vv;
    real_kinds::sp up1, down1, left1, right1;
    real_kinds::sp d_up, d_down, d_left, d_right;
    real_kinds::sp vorticity;
    std::vector<real_kinds::sp> phi(bgk_storage.m1 + 2); //(-1, m1) => m1 + 1 + 1
    real_kinds::sp cte1;

    std::string file_name = "tec_om.xxxxxxxx.vtk";

    bgk_storage.myrank = 0;

#ifdef NOSHIFT
    cte1 = storage::zero;
#else
    cte1 = storage::uno;
#endif

    file_name.replace(7, 8, fmt::format("{:08d}", itime));
    std::ofstream file(file_name, std::ios::binary);

    //Ported from -1 to 0
    phi[0] = 0.0;
    int i = 1;
    for(int j = 0; j <= bgk_storage.m; ++j) {
        den = (bgk_storage.a01_host(i, j) + bgk_storage.a03_host(i, j) + bgk_storage.a05_host(i, j) + bgk_storage.a08_host(i, j) + bgk_storage.a10_host(i, j) + bgk_storage.a12_host(i, j) + bgk_storage.a14_host(i, j) + bgk_storage.a17_host(i, j)
                  + bgk_storage.a19_host(i, j))
            + cte1;

        uv = bgk_storage.a01_host(i, j) + bgk_storage.a03_host(i, j) + bgk_storage.a05_host(i, j) - bgk_storage.a10_host(i, j) - bgk_storage.a12_host(i, j) - bgk_storage.a14_host(i, j);

        phi[j + 1] = phi[j - 1 + 1] + uv / den;
    }

    file << "# vtk DataFile Version 2.0\n";
    file << "Campo\n";
    file << "BINARY\n";
    file << "DATASET STRUCTURED_POINTS\n";
    file << fmt::format("DIMENSIONS {} {} {}\n", bgk_storage.l - 2, bgk_storage.m - 2, 1);
    file << fmt::format("ORIGIN {} {} {}\n", bgk_storage.offset[0] + 2, bgk_storage.offset[1] + 2, 1);
    file << fmt::format("SPACING {} {} {}\n", 1, 1, 1);
    file << fmt::format("POINT_DATA {}\n", (bgk_storage.l - 2) * (bgk_storage.m - 2));
    file << "SCALARS vorticity float\n";
    file << "LOOKUP_TABLE default\n";
    file.close();

    std::ofstream bin_file(file_name, std::ios::binary | std::ios::app);

    for(int j = 2; j <= bgk_storage.m - 1; ++j) {
        for(int i = 2; i <= bgk_storage.l - 1; ++i) {
            d_up = (bgk_storage.a01_host(i, j + 1) + bgk_storage.a03_host(i, j + 1) + bgk_storage.a05_host(i, j + 1) + bgk_storage.a08_host(i, j + 1) + bgk_storage.a10_host(i, j + 1) + bgk_storage.a12_host(i, j + 1)
                       + bgk_storage.a14_host(i, j + 1) + bgk_storage.a17_host(i, j + 1) + bgk_storage.a19_host(i, j + 1))
                + cte1;

            d_down = (bgk_storage.a01_host(i, j - 1) + bgk_storage.a03_host(i, j - 1) + bgk_storage.a05_host(i, j - 1) + bgk_storage.a08_host(i, j - 1) + bgk_storage.a10_host(i, j - 1) + bgk_storage.a12_host(i, j - 1)
                         + bgk_storage.a14_host(i, j - 1) + bgk_storage.a17_host(i, j - 1) + bgk_storage.a19_host(i, j - 1))
                + cte1;

            d_left = (bgk_storage.a01_host(i - 1, j) + bgk_storage.a03_host(i - 1, j) + bgk_storage.a05_host(i - 1, j) + bgk_storage.a08_host(i - 1, j) + bgk_storage.a10_host(i - 1, j) + bgk_storage.a12_host(i - 1, j)
                         + bgk_storage.a14_host(i - 1, j) + bgk_storage.a17_host(i - 1, j) + bgk_storage.a19_host(i - 1, j))
                + cte1;

            d_right = (bgk_storage.a01_host(i + 1, j) + bgk_storage.a03_host(i + 1, j) + bgk_storage.a05_host(i + 1, j) + bgk_storage.a08_host(i + 1, j) + bgk_storage.a10_host(i + 1, j) + bgk_storage.a12_host(i + 1, j)
                          + bgk_storage.a14_host(i + 1, j) + bgk_storage.a17_host(i + 1, j) + bgk_storage.a19_host(i + 1, j))
                + cte1;

            up1 = (bgk_storage.a01_host(i, j + 1) + bgk_storage.a03_host(i, j + 1) + bgk_storage.a05_host(i, j + 1) - bgk_storage.a10_host(i, j + 1) - bgk_storage.a12_host(i, j + 1) - bgk_storage.a14_host(i, j + 1))
                / d_up;

            down1 = (bgk_storage.a01_host(i, j - 1) + bgk_storage.a03_host(i, j - 1) + bgk_storage.a05_host(i, j - 1) - bgk_storage.a10_host(i, j - 1) - bgk_storage.a12_host(i, j - 1) - bgk_storage.a14_host(i, j - 1))
                / d_down;

            left1 = (bgk_storage.a03_host(i - 1, j) + bgk_storage.a08_host(i - 1, j) + bgk_storage.a12_host(i - 1, j) - bgk_storage.a01_host(i - 1, j) - bgk_storage.a10_host(i - 1, j) - bgk_storage.a17_host(i - 1, j))
                / d_left;

            right1 = (bgk_storage.a03_host(i + 1, j) + bgk_storage.a08_host(i + 1, j) + bgk_storage.a12_host(i + 1, j) - bgk_storage.a01_host(i + 1, j) - bgk_storage.a10_host(i + 1, j) - bgk_storage.a17_host(i + 1, j))
                / d_right;

            vorticity = 0.5f * (up1 - down1) - 0.5f * (right1 - left1);
            bin_file.write(reinterpret_cast<const char *>(&vorticity), sizeof(vorticity));
        }
    }
    bin_file.close();

    std::ofstream stream_file(file_name, std::ios::app);
    stream_file << "SCALARS stream float\n";
    stream_file << "LOOKUP_TABLE default\n";
    stream_file.close();

    std::ofstream bin_stream_file(file_name, std::ios::binary | std::ios::app);

    for(int j = 2; j <= bgk_storage.m - 1; ++j) {
        for(int i = 2; i <= bgk_storage.l - 1; ++i) {
            den = (bgk_storage.a01_host(i, j) + bgk_storage.a03_host(i, j) + bgk_storage.a05_host(i, j) + bgk_storage.a08_host(i, j) + bgk_storage.a10_host(i, j) + bgk_storage.a12_host(i, j) + bgk_storage.a14_host(i, j) + bgk_storage.a17_host(i, j)
                      + bgk_storage.a19_host(i, j))
                + cte1;

            vv = bgk_storage.a03_host(i, j) + bgk_storage.a08_host(i, j) + bgk_storage.a12_host(i, j) - bgk_storage.a01_host(i, j) - bgk_storage.a10_host(i, j) - bgk_storage.a17_host(i, j);

            phi[j + 1] = phi[j + 1] - vv / den;
            bin_stream_file.write(reinterpret_cast<const char *>(&phi[j]), sizeof(phi[j]));
        }
    }
    bin_stream_file.close();

    std::cout << "I/O: vorticity xy (vtk) done" << "\n";
    auto& file_manager = debug::file_manager::instance();
    file_manager.write(16, "I/O: vorticity xy (vtk) done\n");

#ifdef DEBUG_1
    if(bgk_storage.myrank == 0) { std::cout << "DEBUG1: Exiting from sub. vtk_om_bin" << "\n"; }
#endif


}
} // namespace bgk