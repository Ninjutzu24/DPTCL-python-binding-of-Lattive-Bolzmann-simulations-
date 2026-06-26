#include "save_raw.hh"
#include "time.hh"
#include <save.hh>
#include <file_manager.hh>

namespace bgk{
    void save(storage& bgk_storage, const int itime)
    {
        auto& file_manager = debug::file_manager::instance();
        auto& timing = utils::timing::instance();

        // file_manager.write(0, utils::get_date());
        utils::time(timing.tcountF0);
        utils::system_clock(timing.countF0, timing.count_rate, timing.count_max);

        save_raw(bgk_storage, itime);

        utils::time(timing.tcountF1);
        utils::system_clock(timing.countF1, timing.count_rate, timing.count_max);
        timing.time_io = timing.time_io + static_cast<float>((timing.tcountF1 - timing.tcountF0) / timing.count_rate);
        timing.time_io1 = timing.time_io1 + (timing.tcountF1 - timing.tcountF0);

# ifdef MEM_CHECK
      if(bgk_storage.myrank == 0){
         timing.mem_stop = get_mem();
         fmt::print("MEM_CHECK: after sub. save mem ={}", mem_stop);
      }
# endif

    }
}