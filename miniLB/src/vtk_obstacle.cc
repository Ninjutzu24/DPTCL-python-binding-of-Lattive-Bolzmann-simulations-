#include <fstream>
#include <iomanip>
#include <iostream>
#include <storage.hh>
#include <string>

namespace bgk {

void vtk_obs(storage &bgk_storage) {
    int itime = 0;
    std::string file_nameVTK = "tec_ob.xxxxxxxx.vtk";

#ifdef NO_OUTPUT
    if(bgk_storage.myrank == 0) { std::cout << "INFO: no output mode enabled, no dump at all" << "\n"; }
#endif

    std::cout << "task " << bgk_storage.myrank << " has " << bgk_storage.nobs << " obstacles" << "\n";

    std::ostringstream oss;
    oss << std::setw(8) << std::setfill('0') << itime;
    file_nameVTK.replace(7, 8, oss.str());

    std::cout << "INFO: obstacle vtk dump " << file_nameVTK << "\n";

    std::ofstream file(file_nameVTK);
    if(!file.is_open()) {
        std::cerr << "Error opening file: " << file_nameVTK << "\n";
        return;
    }

    file << "# vtk DataFile Version 2.0" << "\n";
    file << "Campo" << "\n";
    file << "ASCII" << "\n";
    file << "DATASET RECTILINEAR_GRID" << "\n";
    file << "DIMENSIONS " << bgk_storage.l << " " << bgk_storage.m << " 1" << "\n";

    file << "X_COORDINATES " << bgk_storage.l << " double" << "\n";
    for(int i = 1; i <= bgk_storage.l; ++i) { file << i << "\n"; }

    file << "Y_COORDINATES " << bgk_storage.m << " double" << "\n";
    for(int j = 1; j <= bgk_storage.m; ++j) { file << j << "\n"; }

    file << "Z_COORDINATES 1 double" << "\n";
    file << 1 << "\n";

    file << "POINT_DATA " << bgk_storage.l * bgk_storage.m << "\n";
    file << "SCALARS obs float" << "\n";
    file << "LOOKUP_TABLE default" << "\n";
    for(int j = 1; j <= bgk_storage.m; ++j) {
        for(int i = 1; i <= bgk_storage.l; ++i) { file << bgk_storage.obs_host(i - 1, j - 1) * 1.0 << "\n"; }
    }

    std::cout << "INFO: obs vtk dump done" << "\n";
    file.close();

#ifdef DEBUG_1
    if(bgk_storage.myrank == 0) { std::cout << "DEBUG1: Exiting from sub. vtk_obs" << "\n"; }
#endif
}

} // namespace bgk