#include "diagno.hh"
#include "probe_visc.hh"
#include "prof_i.hh"
#include "prof_j.hh"
#include "save.hh"
#include "varm.hh"
#include "vtk_om_binary.hh"
#include "vtk_visc_binary.hh"
#include "vtk_xy.hh"
#include "vtk_xy_binary.hh"
#include <vtk_om.hh>
#include <diagnostic.hh>
#include <time.hh>
#include <probe.hh>
#include <draglift.hh>

namespace bgk {

void diagnostic(storage &bgk_storage, const int itime, const int ivtim, const int icheck, const int itsave) {
    auto& timing = utils::timing::instance();
    if(itime % ivtim == 0) {
        // start timing...
        utils::system_clock(timing.countA0, timing.count_rate, timing.count_max);
        utils::time(timing.tcountA0);

    bgk_storage.update_host();

#ifdef NO_BINARY
        vtk_xy(bgk_storage, itime);
        vtk_om(bgk_storage, itime);
#else
        vtk_xy_bin(bgk_storage, itime);
        vtk_om_bin(bgk_storage, itime);
        vtk_visc_bin(bgk_storage, itime);
#endif

        // stop timing
        utils::time(timing.tcountA1);
        utils::system_clock(timing.countA1, timing.count_rate, timing.count_max);
        timing.time_dg = static_cast<float>(timing.countA1 - timing.countA0) / timing.count_rate;
        timing.time_dg1 += (timing.tcountA1 - timing.tcountA0);
    }

    if(itime % icheck == 0) {
        // start timing...
        utils::system_clock(timing.countA0, timing.count_rate, timing.count_max);
        utils::time(timing.tcountA0);
        
        //Avoid double update
        if (! (itime % ivtim == 0)){
            bgk_storage.update_host();
        }

        diagno(bgk_storage, itime);
        probe(bgk_storage, itime, bgk_storage.l / 2, bgk_storage.m / 2);
        varm(bgk_storage,itime);
        prof_i(bgk_storage, itime, bgk_storage.m / 2);
        prof_j(bgk_storage, itime, bgk_storage.l / 2);

#ifdef DRAG
        draglift(bgk_storage, itime); //TODO: THIS HAD AN ADDITIONAL 333 ARGUMENT, WHAT'S THE MEANING?
#endif

// #ifdef LES
        probe_visc(bgk_storage, itime, bgk_storage.l / 2, bgk_storage.m / 2);
// #endif

        // stop timing
        utils::time(timing.tcountA1);
        utils::system_clock(timing.countA1, timing.count_rate, timing.count_max);
        timing.time_dg += static_cast<float>(timing.countA1 - timing.countA0) / timing.count_rate;
        timing.time_dg1 += (timing.tcountA1 - timing.tcountA0);
    }

    if(itime % itsave == 0) { 
        save(bgk_storage, itime); 
    }

#ifdef DEBUG_2
    if(bgk_storage.myrank == 0) {
        if(itime % icheck == 0) {
            // write(6,*) "DEBUG2: Exiting from sub. diagnostic", itime
            // write(6,*) "DEBUG2", int(2.0*l/5.0),int(m/2.0)
            // write(6,*) "DEBUG2", icoord, jcoord
            // write(6,*) "DEBUG2", radius, delta
            // write(6,*) "DEBUG2", istart, istop
            // write(6,*) "DEBUG2", jstart, jstop
            // write(6,*) "DEBUG2", fluxX, fluxY
        } else {
            std::cout << "DEBUG2: Exiting from sub. diagnostic " << itime << "\n";
        }
    }
#endif
}

} // namespace bgk