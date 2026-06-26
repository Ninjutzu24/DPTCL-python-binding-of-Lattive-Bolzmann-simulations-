#include <file_manager.hh>
#include <fmt/core.h>
#include <profile.hh>
#include <time.hh>

namespace bgk {
void profile(storage &bgk_storage, const int itime, const int itfin, const int isignal) {
    auto &timing = utils::timing::instance();
    auto &file_manager = debug::file_manager::instance();

    utils::system_clock(timing.countD1, timing.count_rate, timing.count_max);
    utils::time(timing.tcountD1);
    timing.time_inn_loop = static_cast<float>(timing.countD1 - timing.countD0) / timing.count_rate;
    timing.time_inn_loop1 = (timing.tcountD1 - timing.tcountD0);
    fmt::print(" Mean   time {:14.6e} {:8d}/{:8d}\n", timing.time_inn_loop / static_cast<float>(isignal), itime, itfin);

#ifdef MEM_CHECK
    timing.mem_stop = get_mem();
    fmt::print("MEM_CHECK: iteration {} mem = {}\n", itime, timing.mem_stop);
#endif

// some info about time...
#ifdef CRAY
    file_manager.write_format<"{} {} {} {} {} {} {}\n">(99, itime, timing.time_bc - timing.old1,
        timing.time_coll - timing.old2, timing.time_move - timing.old3, timing.time_dg - timing.old4,
        timing.time_io - timing.old5, timing.time_obs - timing.old6);

#else
    file_manager.write_format<"{:8d} {:14.6e} {:14.6e} {:14.6e} {:14.6e} {:14.6e} {:14.6e}\n">(99, itime,
        timing.time_bc - timing.old1, timing.time_coll - timing.old2, timing.time_move - timing.old3,
        timing.time_dg - timing.old4, timing.time_io - timing.old5, timing.time_obs - timing.old6);
#endif

    timing.old1 = timing.time_bc;
    timing.old2 = timing.time_coll;
    timing.old3 = timing.time_move;
    timing.old4 = timing.time_dg;
    timing.old5 = timing.time_io;
    timing.old6 = timing.time_obs;

    utils::system_clock(timing.countD0, timing.count_rate, timing.count_max);
    utils::time(timing.tcountD0);
}
} // namespace bgk