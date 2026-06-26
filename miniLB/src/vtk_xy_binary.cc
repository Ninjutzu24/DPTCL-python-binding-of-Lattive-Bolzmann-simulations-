#include <iostream>
#include <vtk_xy_binary.hh>
#include <sstream>
#include <string>
#include <fstream>
#include <iomanip>
#include <file_manager.hh>

namespace bgk {

void vtk_xy_bin(storage &bgk_storage, const int itime) {
  // Define necessary variables
    std::string file_name = "tec_xy.xxxxxxxx.vtk";
    real_kinds::sp u;
    real_kinds::sp w;
    real_kinds::sp den;
    real_kinds::sp cte1;
    real_kinds::sp zerol = 0.0;
    bgk_storage.myrank = 0;

    // Format the file name with itime
    file_name.replace(7, 8, std::to_string(itime));

    // Determine the value of cte1 based on the NOSHIFT flag
#ifdef NOSHIFT
    cte1 = storage::zero;
#else
    cte1 = storage::uno;
#endif

    // Write the ASCII header to the file
    std::ofstream file(file_name, std::ios::out);
    file << "# vtk DataFile Version 2.0\n";
    file << "Campo\n";
    file << "BINARY\n";
    file << "DATASET STRUCTURED_POINTS\n";
    file << "DIMENSIONS " << bgk_storage.l << " " << bgk_storage.m << " 1\n";
    file << "ORIGIN " << bgk_storage.offset[0] + 1 << " " << bgk_storage.offset[1] + 1 << " 1\n";
    file << "SPACING 1 1 1\n";
    file << "POINT_DATA " << bgk_storage.l * bgk_storage.m * 1 << "\n";
    file << "VECTORS velocity float\n";
    file.close();

    // Write the binary data for vectors u and w
    std::ofstream bin_file(file_name, std::ios::out | std::ios::binary | std::ios::app);
    for (int j = 1; j <= bgk_storage.m; ++j) {
        for (int i = 1; i <= bgk_storage.l; ++i) {
            u = bgk_storage.a01_host(i,j) + bgk_storage.a03_host(i,j) + bgk_storage.a05_host(i,j) - bgk_storage.a10_host(i,j) - bgk_storage.a12_host(i,j) - bgk_storage.a14_host(i,j);
            w = bgk_storage.a03_host(i,j) + bgk_storage.a08_host(i,j) + bgk_storage.a12_host(i,j) - bgk_storage.a01_host(i,j) - bgk_storage.a10_host(i,j) - bgk_storage.a17_host(i,j);
            den = bgk_storage.a01_host(i,j) + bgk_storage.a03_host(i,j) + bgk_storage.a05_host(i,j) + bgk_storage.a08_host(i,j) + bgk_storage.a10_host(i,j) + bgk_storage.a12_host(i,j) + bgk_storage.a14_host(i,j) + bgk_storage.a17_host(i,j) + bgk_storage.a19_host(i,j) + cte1;
            const auto u_den = u / den;
            const auto w_den = w / den;
            bin_file.write(reinterpret_cast<const char*>(&u_den), sizeof(u_den));
            bin_file.write(reinterpret_cast<const char*>(&w_den), sizeof(w_den));
            bin_file.write(reinterpret_cast<char*>(&zerol), sizeof(zerol));
        }
    }
    bin_file.close();

    // Write the scalar data for w
    file.open(file_name, std::ios::out | std::ios::app);
    file << "SCALARS w float\n";
    file << "LOOKUP_TABLE default\n";
    file.close();

    bin_file.open(file_name, std::ios::out | std::ios::binary | std::ios::app);
    for (int j = 1; j <= bgk_storage.m; ++j) {
        for (int i = 1; i <= bgk_storage.l; ++i) {
            w = bgk_storage.a03_host(i,j) + bgk_storage.a08_host(i,j) + bgk_storage.a12_host(i,j) - bgk_storage.a01_host(i,j) - bgk_storage.a10_host(i,j) - bgk_storage.a17_host(i,j);
            const auto w_den = w / den;
            bin_file.write(reinterpret_cast<const char*>(&w_den), sizeof(w_den));
        }
    }
    bin_file.close();

    // Write the scalar data for u
    file.open(file_name, std::ios::out | std::ios::app);
    file << "SCALARS u float\n";
    file << "LOOKUP_TABLE default\n";
    file.close();

    bin_file.open(file_name, std::ios::out | std::ios::binary | std::ios::app);
    for (int j = 1; j <= bgk_storage.m; ++j) {
        for (int i = 1; i <= bgk_storage.l; ++i) {
            u = bgk_storage.a01_host(i,j) + bgk_storage.a03_host(i,j) + bgk_storage.a05_host(i,j) - bgk_storage.a10_host(i,j) - bgk_storage.a12_host(i,j) - bgk_storage.a14_host(i,j);
            const auto u_den = u / den;
            bin_file.write(reinterpret_cast<const char*>(&u_den), sizeof(u_den));
        }
    }
    bin_file.close();

    // Write the scalar data for den
    file.open(file_name, std::ios::out | std::ios::app);
    file << "SCALARS rho float\n";
    file << "LOOKUP_TABLE default\n";
    file.close();

    bin_file.open(file_name, std::ios::out | std::ios::binary | std::ios::app);
    for (int j = 1; j <= bgk_storage.m; ++j) {
        for (int i = 1; i <= bgk_storage.l; ++i) {
            den = bgk_storage.a01_host(i,j) + bgk_storage.a03_host(i,j) + bgk_storage.a05_host(i,j) + bgk_storage.a08_host(i,j) + bgk_storage.a10_host(i,j) + bgk_storage.a12_host(i,j) + bgk_storage.a14_host(i,j) + bgk_storage.a17_host(i,j) + bgk_storage.a19_host(i,j) + cte1;
            bin_file.write(reinterpret_cast<char*>(&den), sizeof(den));
        }
    }
    bin_file.close();

    std::cout << "I/O: plane xy (vtk,binary) done" << "\n";
    auto& file_manager = debug::file_manager::instance();
    file_manager.write(16, "I/O: plane xy (vtk,binary) done\n");
    
    #ifdef DEBUG_1
        if(bgk_storage.myrank == 0){
        std::cout << "DEBUG1: Exiting from sub. vtk_xy_bin\n";
        }
#endif
}

} // namespace bgk