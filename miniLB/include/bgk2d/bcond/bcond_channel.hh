#ifndef __BCOND_CHANNEL_H__
#define __BCOND_CHANNEL_H__

#include <storage.hh>

namespace bgk{
//    =====================================================================
//      ****** LBE/bcond_channel
// 
//      COPYRIGHT
//        (c) 2000-2011 by CASPUR/G.Amati
//        (c) 2013-20?? by CINECA/G.Amati
//      NAME
//        bcond_channel: simple (periodic) channel flow with
//                       * periodic (rear)
//                       * periodic (front)
//                       * no slip (left/right)
//      DESCRIPTION
//        2D periodic bc
//      INPUTS
//        none
//      OUTPUT
//        none
//      TODO
//        
//      NOTES
//        Order of upgrading bc
//        1) front (x = l)
//        2) rear  (x = 0)
//        3) left  (y = 0)
//        4) right (y = m)
// 
//      *****
// =====================================================================
    void bcond_channel(storage& bgk_storage);
}
#endif // __BCOND_CHANNEL_H__