#include "bcond/bcond_channel.hh"
#include "bcond/bcond_inflow.hh"
#include <bcond/boundaries.hh>
#include <bcond/bcond_driven.hh>
#include <bcond/bcond_obs.hh>
#include <bcond/bcond_periodic.hh>


#include <fmt/core.h>



namespace bgk {



void boundaries(storage &bgk_storage) {
    // Different boundary conditions (selected at compile-time)
#ifdef PERIODIC
    // Periodic boundary condition
    bcond_periodic(bgk_storage);
#elif CHANNEL
    // Channel boundary condition
    bcond_channel(bgk_storage);
#elif INFLOW
    // Inflow boundary condition
    bcond_inflow(bgk_storage);
#elif MYVERSION
    // Periodic boundary condition (my version)
    bcond_periodic(bgk_storage);
#else
    // Lid-driven cavity boundary condition (default)
    bcond_driven(bgk_storage);
#endif

#ifdef OBSTACLE
    bcond_obs(bgk_storage);
#endif

#ifdef DEBUG_2
    if(bgk_storage.myrank == 0) { fmt::pripnt("DEBUG2: Exiting from function boundaries\n"); }
#endif
}
} // namespace bgk