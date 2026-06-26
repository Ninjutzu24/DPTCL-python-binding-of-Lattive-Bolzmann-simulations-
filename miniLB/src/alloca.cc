#include <iostream>
#include <fmt/core.h>

#include <alloca.hh>
#include <get_mem.hh>

namespace bgk {

void alloc(storage &bgk_storage) {
    bgk_storage.init();

    long mem_start = 0;
    long mem_stop = 0;

#ifndef PGI
    mem_start = get_mem();
#endif

#ifdef DEBUG_1
    if(bgk_storage.myrank == 0) { fmt::print("DEBUG1: Exiting from sub. alloca\n"); }
#endif

#ifdef MEM_CHECK
    if(bgk_storage.myrank == 0) {
        mem_stop = get_mem();
        fmt::print("MEM_CHECK: after sub. alloca mem = {}\n", mem_stop);
    }
#endif
}

} // namespace bgk