#include <restore_raw.hh>

#include <fstream>
#include <iostream>
#include <file_manager.hh>
#include <time.hh>

namespace bgk{
    
    void restore_raw(storage& bgk_storage, int& itime){
            std::string file_name = "restore.bin";
    std::ifstream file(file_name, std::ios::binary);
    auto& file_manager = debug::file_manager::instance();

    if (!file.is_open()) {
        std::cerr << "Error opening file: " << file_name << "\n";
        return;
    }

    file_manager.write(16, "INFO: I am restoring from", file_name);
    std::cout << "INFO: I am restoring from " << file_name << "\n";

    file.read(reinterpret_cast<char*>(&itime), sizeof(itime));

    for (int j = 0; j <= bgk_storage.m1; ++j) {
        for (int i = 0; i <= bgk_storage.l1; ++i) {
            file.read(reinterpret_cast<char*>(&bgk_storage.a01_host(i,j)), sizeof(double));
        }
    }
    for (int j = 0; j <= bgk_storage.m1; ++j) {
        for (int i = 0; i <= bgk_storage.l1; ++i) {
            file.read(reinterpret_cast<char*>(&bgk_storage.a03_host(i,j)), sizeof(double));
        }
    }
    for (int j = 0; j <= bgk_storage.m1; ++j) {
        for (int i = 0; i <= bgk_storage.l1; ++i) {
            file.read(reinterpret_cast<char*>(&bgk_storage.a05_host(i,j)), sizeof(double));
        }
    }
    for (int j = 0; j <= bgk_storage.m1; ++j) {
        for (int i = 0; i <= bgk_storage.l1; ++i) {
            file.read(reinterpret_cast<char*>(&bgk_storage.a08_host(i,j)), sizeof(double));
        }
    }
    for (int j = 0; j <= bgk_storage.m1; ++j) {
        for (int i = 0; i <= bgk_storage.l1; ++i) {
            file.read(reinterpret_cast<char*>(&bgk_storage.a10_host(i,j)), sizeof(double));
        }
    }
    for (int j = 0; j <= bgk_storage.m1; ++j) {
        for (int i = 0; i <= bgk_storage.l1; ++i) {
            file.read(reinterpret_cast<char*>(&bgk_storage.a12_host(i,j)), sizeof(double));
        }
    }
    for (int j = 0; j <= bgk_storage.m1; ++j) {
        for (int i = 0; i <= bgk_storage.l1; ++i) {
            file.read(reinterpret_cast<char*>(&bgk_storage.a14_host(i,j)), sizeof(double));
        }
    }
    for (int j = 0; j <= bgk_storage.m1; ++j) {
        for (int i = 0; i <= bgk_storage.l1; ++i) {
            file.read(reinterpret_cast<char*>(&bgk_storage.a17_host(i,j)), sizeof(double));
        }
    }
    for (int j = 0; j <= bgk_storage.m1; ++j) {
        for (int i = 0; i <= bgk_storage.l1; ++i) {
            file.read(reinterpret_cast<char*>(&bgk_storage.a19_host(i,j)), sizeof(double));
        }
    }

    file.close();

#ifdef DEBUG_1
    if (bgk_storage.myrank == 0) {
        std::cout << "DEBUG1: Exiting from sub. restore_raw" << "\n";
    }
#endif
#ifdef MEM_CHECK
    if (bgk_storage.myrank == 0) {
        auto& timing = utils::timing::instance();
        // Assuming get_mem() is a function that returns memory usage
        timing.mem_stop = get_mem();
        std::cout << "MEM_CHECK: after sub. restore_raw mem = " << mem_stop << "\n";
    }
#endif
    }
}