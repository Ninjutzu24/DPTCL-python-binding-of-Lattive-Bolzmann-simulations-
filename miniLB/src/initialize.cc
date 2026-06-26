#include <bcond/build_bcond.hh>
#include <build_obs.hh>
#include <initialize.hh>
#include <iostream>
#include <time.hh>
#include <vtk_obstacle.hh>
#include "outdat.hh"
#include "prof_j.hh"
#include "restore_raw.hh"
#include "varm.hh"
#include "vtk_xy_binary.hh"
#include "w_obs.hh"
#include <init.hh>
#include <prof_i.hh>
#include <hencol.hh>

namespace bgk {


void initialize(storage &bgk_storage, const int itrestart, const int init_v, const int itfin, int &itstart,
    const int ivtim, const int isignal, const int itsave, const int icheck) {
    auto &timing = utils::timing::instance();

    // Original some other initializations
    timing.time_coll = 0.0;
    timing.time_move = 0.0;
    timing.time_obs = 0.0;
    timing.time_bc = 0.0;
    timing.time_mp = 0.0;
    timing.time_dg = 0.0;
    timing.time_io = 0.0;
    timing.time_dev = 0.0;
    timing.time_coll1 = 0.0;
    timing.time_move1 = 0.0;
    timing.time_obs1 = 0.0;
    timing.time_bc1 = 0.0;
    timing.time_mp1 = 0.0;
    timing.time_dg1 = 0.0;
    timing.time_io1 = 0.0;
    timing.time_dev1 = 0.0;
    timing.timeZ = 0.0;
    timing.timeY = 0.0;
    timing.timeX = 0.0;
    timing.old1 = 0.0;
    timing.old2 = 0.0;
    timing.old3 = 0.0;
    timing.old4 = 0.0;
    timing.old5 = 0.0;
    timing.old6 = 0.0;

    // set boundary condition flags..
    build_bcond(bgk_storage);

// build obstacles
#ifdef OBSTACLE
    build_obs(bgk_storage);
    vtk_obs(bgk_storage);
    // to fix
    w_obs(bgk_storage); // write ASCII file
                        // call r_obs              // read ASCII file
#endif

    if(itrestart == 1) {
        utils::time(timing.tcountH0);
        utils::system_clock(timing.countF0, timing.count_rate, timing.count_max);

        if(bgk_storage.myrank == 0) {
            restore_raw(bgk_storage, itstart);
            std::cout << "INFO: restoring at timestep " << itstart << "\n";
        }

        utils::system_clock(timing.countF1, timing.count_rate, timing.count_max);
        utils::time(timing.tcountF1);
        timing.time_io += static_cast<double>(timing.countF1 - timing.countF0) / timing.count_rate;
        timing.time_io1 += timing.tcountF1 - timing.tcountF0;

#ifdef DEBUG_1
        if(bgk_storage.myrank == 0) {
            std::cout << "DEBUG1: I/O time (1) "
                      << static_cast<double>(timing.countF1 - timing.countF0) / timing.count_rate << "\n";
            std::cout << "DEBUG1: I/O time (2) " << timing.tcountF1 - timing.tcountF0 << "\n";
        }
#endif

    } else if(itrestart >= 2) {
        itstart = 0;
        std::cerr << "still not implemented" << "\n";
        std::exit(EXIT_FAILURE);
    } else {
        itstart = 0;
        init(bgk_storage, init_v);
        // diagno(itstart); // Uncomment if needed
        varm(bgk_storage, itstart);
        prof_i(bgk_storage, itstart, bgk_storage.m / 2);
        prof_j(bgk_storage, itstart, bgk_storage.l / 2);
#ifdef NO_BINARY
        vtk_xy(bgk_storage, itstart);
#else
        vtk_xy_bin(bgk_storage, itstart);
#endif
    }

    // compute collision parameters
    hencol(bgk_storage);

    if(bgk_storage.myrank == 0)
    { 
        outdat(bgk_storage, itfin, itstart, ivtim, isignal, itsave, icheck); 
    }

#ifdef NO_OUTPUT
// do nothing
#else
// prof_i(0, m / 2); // Uncomment if needed
// prof_j(0, l / 2); // Uncomment if needed
#endif

#ifdef DEBUG_1
    if(bgk_storage.myrank == 0) { std::cout << "DEBUG1: Exiting from sub. initialize" << "\n"; }
#endif

#ifdef MEM_CHECK
    if(myrank == 0) {
        timing.mem_stop = get_mem();
        std::cout << "MEM_CHECK: after initialize. mem = " << timing.mem_stop << "\n";
    }
#endif
}


} // namespace bgk