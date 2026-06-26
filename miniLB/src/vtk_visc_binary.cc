#include <vtk_visc_binary.hh>
#include <iostream>
#include <fstream>
#include <cmath>
#include <fmt/format.h>
#include <file_manager.hh>

namespace bgk {
    
void vtk_visc_bin(storage& bgk_storage, const int itime){

    std::string file_name = "tec_visc.xxxxxxxx.vtk";

    real_kinds::sp u, w, den;
    real_kinds::sp cte1, cte0;
    real_kinds::sp x01, x03, x05, x08, x10;
    real_kinds::sp x12, x14, x17, x19;
    real_kinds::sp e01, e03, e05, e08, e10;
    real_kinds::sp e12, e14, e17, e19;
    real_kinds::sp n01, n03, n05, n08, n10;
    real_kinds::sp n12, n14, n17, n19;
    real_kinds::sp rho, rhoinv, vx, vy, vx2, vy2, vsq;
    real_kinds::sp vxpy, vxmy, rp1, rp2, rp0;
    real_kinds::sp qxpy, qxmy, qx, qy, q0;
    real_kinds::sp Pxx, Pxy, Pyx, Pyy, Ptotal;
    real_kinds::sp omega2;
    real_kinds::sp Ts;
    
    bgk_storage.myrank = 0;

#ifdef NOSHIFT
    cte1 = storage::zero;
#else
    cte1 = storage::uno;
#endif
    cte0 = storage::uno - cte1;

    file_name.replace(9, 8, fmt::format("{:08}", itime));

    std::ofstream file(file_name, std::ios::out | std::ios::trunc);
    if (!file) {
        std::cerr << "Error opening file: " << file_name << "\n";
        return;
    }

    file << "# vtk DataFile Version 2.0\n";
    file << "Campo\n";
    file << "BINARY\n";
    file << "DATASET STRUCTURED_POINTS\n";
    file << fmt::format("DIMENSIONS {} {} {}\n", bgk_storage.l, bgk_storage.m, 1);
    file << fmt::format("ORIGIN {} {} {}\n", bgk_storage.offset[0] + 1, bgk_storage.offset[1] + 1, 1);
    file << fmt::format("SPACING {} {} {}\n", 1, 1, 1);
    file << fmt::format("POINT_DATA {}\n", bgk_storage.l * bgk_storage.m * 1);
    file << "SCALARS viscosity float\n";
    file << "LOOKUP_TABLE default\n";
    file.close();

    std::ofstream bin_file(file_name, std::ios::binary | std::ios::app);
    if (!bin_file) {
        std::cerr << "Error opening file: " << file_name << "\n";
        return;
    }
    //TODO: Offload??

    for (int j = 1; j <= bgk_storage.m; ++j) {
        for (int i = 1; i <= bgk_storage.l; ++i) {
            x01 = bgk_storage.a01_host(i,j);
            x03 = bgk_storage.a03_host(i,j);
            x05 = bgk_storage.a05_host(i,j);
            x08 = bgk_storage.a08_host(i,j);
            x10 = bgk_storage.a10_host(i,j);
            x12 = bgk_storage.a12_host(i,j);
            x14 = bgk_storage.a14_host(i,j);
            x17 = bgk_storage.a17_host(i,j);
            x19 = bgk_storage.a19_host(i,j);

            rho = (x01 + x03 + x05 + x08 + x10 + x12 + x14 + x17 + x19) + cte1;

            vx = (x01 + x03 + x05 - x10 - x12 - x14) / rho;
            vy = (x03 + x08 + x12 - x01 - x10 - x17) / rho;

            vx2 = vx * vx;
            vy2 = vy * vy;
            vsq = vx2 + vy2;

            vxpy = vx + vy;
            vxmy = vx - vy;

            qxpy = cte0 + storage::qf * (storage::tre * vxpy * vxpy - vsq);
            qxmy = cte0 + storage::qf * (storage::tre * vxmy * vxmy - vsq);
            qx = cte0 + storage::qf * (storage::tre * vx2 - vsq);
            qy = cte0 + storage::qf * (storage::tre * vy2 - vsq);
            q0 = cte0 + storage::qf * (-vsq);

            vx = storage::rf * vx;
            vy = storage::rf * vy;
            vxpy = storage::rf * vxpy;
            vxmy = storage::rf * vxmy;

            rp0 = rho * storage::p0;
            rp1 = rho * storage::p1;
            rp2 = rho * storage::p2;

            e01 = rp2 * (vxmy + qxmy) + cte1 * (rp2 - storage::p2);
            e03 = rp2 * (vxpy + qxpy) + cte1 * (rp2 - storage::p2);
            e05 = rp1 * (vx + qx) + cte1 * (rp1 - storage::p1);
            e08 = rp1 * (vy + qy) + cte1 * (rp1 - storage::p1);
            e10 = rp2 * (-vxpy + qxpy) + cte1 * (rp2 - storage::p2);
            e12 = rp2 * (-vxmy + qxmy) + cte1 * (rp2 - storage::p2);
            e14 = rp1 * (-vx + qx) + cte1 * (rp1 - storage::p1);
            e17 = rp1 * (-vy + qy) + cte1 * (rp1 - storage::p1);
            e19 = rp0 * (q0) + cte1 * (rp0 - storage::p0);

            n01 = x01 - e01;
            n03 = x03 - e03;
            n05 = x05 - e05;
            n08 = x08 - e08;
            n10 = x10 - e10;
            n12 = x12 - e12;
            n14 = x14 - e14;
            n17 = x17 - e17;

            Pxx = n01 + n03 + n05 + n10 + n12 + n14;
            Pyy = n01 + n03 + n08 + n10 + n12 + n17;
            Pxy = -n01 + n03 + n10 - n12;
            Pyx = Pxy;

            Ptotal = std::sqrt(Pxx * Pxx + 2 * Pxy * Pyx + Pyy * Pyy);

            Ts = 1 / (2 * bgk_storage.omega1) + std::sqrt(18.0 * (bgk_storage.cteS * bgk_storage.cteS) * Ptotal + (1.0 / bgk_storage.omega1) * (1.0 / bgk_storage.omega1)) / 2;
            omega2 = 1 / Ts;

            float viscosity = (2.0 / omega2 - 1.0) / 6.0;
            bin_file.write(reinterpret_cast<char*>(&viscosity), sizeof(viscosity));
        }
    }
    bin_file.close();

    std::cout << "I/O: viscosity (vtk,binary) done" << "\n";
    auto& file_manager = debug::file_manager::instance();
    file_manager.write(16, "I/O: viscosity (vtk,binary) done\n");

#ifdef DEBUG_1
    if (bgk_storage.myrank == 0) {
        std::cout << "DEBUG1: Exiting from sub. vtk_visc_bin" << "\n";
    }
#endif
}

} // namespace bgk