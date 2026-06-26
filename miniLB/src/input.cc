#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <input.hh>
#include <storage.hh>
#include <file_manager.hh>
#include <time.hh>
#include <utils.hh>

namespace bgk {
                
/**
    Function paramters to read: 
    svisc, u0, itfin, ivtim, isignal, & 
    itsave, icheck, irestart, init_v, &
    lx, ly,                           &
    flag1, flag2, flag3, ipad, jpad,  & 
    radius,cteS
    * 
    */
void readParameters(std::string_view filename, storage &bgk_storage, int &itfin, int &ivtim, int &isignal,
    int &itsave, int &icheck, int &irestart, int &init_v) {
    // clang-format off
    std::ifstream file(filename.data());
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << "\n";
        std::exit(-1);
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string key;
        if (std::getline(iss, key, '=')) {
            std::string value;
            if (std::getline(iss, value)) {
                utils::trim(key);
                utils::trim(value);
                if (key == "lx") bgk_storage.lx = std::stoi(value);
                else if (key == "ly") bgk_storage.ly = std::stoi(value);
                else if (key == "svisc") bgk_storage.svisc = std::stod(value);
                else if (key == "u0") bgk_storage.u0 = std::stod(value);
                else if (key == "itfin") itfin = std::stoi(value);
                else if (key == "ivtim") ivtim = std::stoi(value);
                else if (key == "isignal") isignal = std::stoi(value);
                else if (key == "itsave") itsave = std::stoi(value);
                else if (key == "icheck") icheck = std::stoi(value);
                else if (key == "irestart") irestart = std::stoi(value);
                else if (key == "init_v") init_v = std::stoi(value);
                else if (key == "ipad") bgk_storage.ipad = std::stoi(value);
                 else if (key == "flag1") bgk_storage.flag1 = std::stoi(value);
                 else if (key == "flag2") bgk_storage.flag2 = std::stoi(value);
                 else if (key == "flag3") bgk_storage.flag3 = std::stoi(value);
                 else if (key == "jpad") bgk_storage.jpad = std::stoi(value);
                 else if (key == "radius") bgk_storage.radius = std::stod(value);
                 else if (key == "cteS") bgk_storage.cteS = std::stod(value);
            }
        }
    }

    file.close();
    // clang-format on
}

void input(storage& bgk_storage, int& itfin, int& ivtim, int& isignal, int& itsave, int& icheck, int& irestart, int& init_v) {
    // Setting default values
    bgk_storage.flag1 = 0;         // creating obstacles  (1-file/2-creating)
    bgk_storage.flag2 = 0;         // obstacles           (1-sphere/2-cilinder)
    bgk_storage.flag3 = 0; 
    bgk_storage.cteS  = 0.1;       // smagorinsky constant
    bgk_storage.ipad  = 0;         // no memory padding (x)
    bgk_storage.jpad  = 0;         // no memory padding (y)
    bgk_storage.radius = 16;
    init_v= 0;

    //Read parameter file
    readParameters(files::input_file, bgk_storage, itfin, ivtim, isignal, itsave, icheck, irestart, init_v);

    //Paramter settings from the original code
    bgk_storage.l = bgk_storage.lx;
    bgk_storage.m = bgk_storage.ly;
    
    //ORIGINAL: some fix...
    bgk_storage.l1 = bgk_storage.l + 1;
    bgk_storage.m1 = bgk_storage.m + 1;

    //ORIGINAL: a check
    #ifdef OBSTACLE
    if (bgk_storage.radius > bgk_storage.m / 8) {
        std::cout << "WARNING: radius size = " << bgk_storage.radius << ", " << bgk_storage.m << "\n";
    }

    if (bgk_storage.radius > (bgk_storage.m - 2)) {
        std::cerr << "ERROR: radius too big = " << bgk_storage.radius << ", " << bgk_storage.m << "\n";
        std::exit(EXIT_FAILURE);
    }
#endif

    // Default value: volume forcing along x
    bgk_storage.u0x = decltype(bgk_storage.u0x){0.0};
    bgk_storage.u0y = decltype(bgk_storage.u0y){0.0};

#ifdef DEBUG_1
    if (bgk_storage.myrank == 0) {
        std::cout << "DEBUG1: Exiting from sub. input" << "\n";
    }
#endif

#ifdef MEM_CHECK
    if (storage.myrank == 0) {
        auto& timing = utils::timing::instance();
        timing.mem_stop = get_mem();
        std::cout << "MEM_CHECK: after sub. input mem = " << timing.mem_stop << "\n";
    }
#endif
}


} // namespace bgk