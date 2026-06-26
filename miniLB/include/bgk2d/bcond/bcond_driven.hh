#ifndef __BCOND_DRIVER_H__
#define __BCOND_DRIVER_H__

#include "storage.hh"

/**
 * @brief Lid-driven boundary condition
 * Order of upgrading bc
!       1) front (x = l)
!       2) rear  (x = 0)
!       3) left  (y = 0)
!       4) right (y = m)
 */
namespace bgk{
    void bcond_driven(storage& bgk_storage);
}

#endif // __BCOND_DRIVER_H__