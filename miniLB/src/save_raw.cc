#include <save_raw.hh>
#include "fmt/core.h"
#include <fstream>
#include <file_manager.hh>

namespace bgk{
    void save_raw(storage& bgk_storage, const int itime){
    auto& file_manager = debug::file_manager::instance();
    std::string file_name = fmt::format("save.{:08}.bin", itime);
    std::ofstream file(file_name, std::ios::binary | std::ios::trunc);

    if (bgk_storage.myrank == 0) {
        file_manager.write_format<"task {} saving t= {} {}\n">(16, bgk_storage.myrank, itime, file_name);
        fmt::print("task {} saving t= {} {}\n", bgk_storage.myrank, itime, file_name);
    }

    file.write(reinterpret_cast<const char*>(&itime), sizeof(itime));

    for (int j = 0; j <= bgk_storage.m1; ++j) {
        for (int i = 0; i <= bgk_storage.l1; ++i) {
            file.write(reinterpret_cast<const char*>(&bgk_storage.a01_host(i,j)), sizeof(double));
        }
    }
    for (int j = 0; j <= bgk_storage.m1; ++j) {
        for (int i = 0; i <= bgk_storage.l1; ++i) {
            file.write(reinterpret_cast<const char*>(&bgk_storage.a03_host(i,j)), sizeof(double));
        }
    }
    for (int j = 0; j <= bgk_storage.m1; ++j) {
        for (int i = 0; i <= bgk_storage.l1; ++i) {
            file.write(reinterpret_cast<const char*>(&bgk_storage.a05_host(i,j)), sizeof(double));
        }
    }
    for (int j = 0; j <= bgk_storage.m1; ++j) {
        for (int i = 0; i <= bgk_storage.l1; ++i) {
            file.write(reinterpret_cast<const char*>(&bgk_storage.a08_host(i,j)), sizeof(double));
        }
    }
    for (int j = 0; j <= bgk_storage.m1; ++j) {
        for (int i = 0; i <= bgk_storage.l1; ++i) {
            file.write(reinterpret_cast<const char*>(&bgk_storage.a10_host(i,j)), sizeof(double));
        }
    }
    for (int j = 0; j <= bgk_storage.m1; ++j) {
        for (int i = 0; i <= bgk_storage.l1; ++i) {
            file.write(reinterpret_cast<const char*>(&bgk_storage.a12_host(i,j)), sizeof(double));
        }
    }
    for (int j = 0; j <= bgk_storage.m1; ++j) {
        for (int i = 0; i <= bgk_storage.l1; ++i) {
            file.write(reinterpret_cast<const char*>(&bgk_storage.a14_host(i,j)), sizeof(double));
        }
    }
    for (int j = 0; j <= bgk_storage.m1; ++j) {
        for (int i = 0; i <= bgk_storage.l1; ++i) {
            file.write(reinterpret_cast<const char*>(&bgk_storage.a17_host(i,j)), sizeof(double));
        }
    }
    for (int j = 0; j <= bgk_storage.m1; ++j) {
        for (int i = 0; i <= bgk_storage.l1; ++i) {
            file.write(reinterpret_cast<const char*>(&bgk_storage.a19_host(i,j)), sizeof(double));
        }
    }

    file.flush();
    file.close();

#ifdef DEBUG_1
    if (bgk_storage.myrank == 0) {
        fmt::print("DEBUG1: Exiting from sub. save_raw\n");
    }
#endif
    }
}