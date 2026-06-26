#include "col_MC.hh"
#include <collision.hh>
#include <time.hh>

namespace bgk {
void collision(storage &bgk_storage, const int itime) {
  auto &timing = utils::timing::instance();
  utils::system_clock(timing.countC0, timing.count_rate, timing.count_max);
  utils::time(timing.tcountC0);

#ifdef FUSED
  col_MC(bgk_storage, itime);
#else
  call col(itime) // TODO: unimplemented
#endif

  utils::time(timing.tcountC1);
  utils::system_clock(timing.countC1, timing.count_rate, timing.count_max);
  timing.time_coll = timing.time_coll + static_cast<float>((timing.tcountC1 - timing.tcountC0) / timing.count_rate); 
  timing.time_coll1 = timing.time_coll1 +  (timing.tcountC1 - timing.tcountC0);

  #ifdef DEBUG_2
        if(bgk_storage.myrank == 0) 
           std::cout << "DEBUG2: Exiting from sub. collision";
#endif

}
} // namespace bgk