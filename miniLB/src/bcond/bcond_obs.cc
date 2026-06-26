#include <bcond/bcond_obs.hh>
#include <time.hh>
#include <file_manager.hh>
#include <utils.hh>
namespace bgk {

    void bcond_obs(storage& bgk_storage){
        auto& timing = utils::timing::instance();
        utils::system_clock(timing.countO0, timing.count_rate, timing.count_max);
        utils::time(timing.tcountO0);
        auto& q = *bgk_storage.dev_q;

        #ifndef SYCL_ND_RANGE
        const auto range_j = bgk_storage.jmax - bgk_storage.jmin + 1;
        const auto range_i = bgk_storage.imax - bgk_storage.imin + 1;
        [[maybe_unused]] auto event = q.parallel_for(sycl::range(range_j, range_i), [
            obs = bgk_storage.obs_device,
            a01 = bgk_storage.a01_device,
            a03 = bgk_storage.a03_device,
            a05 = bgk_storage.a05_device,
            a08 = bgk_storage.a08_device,
            a10 = bgk_storage.a10_device,
            a12 = bgk_storage.a12_device,
            a14 = bgk_storage.a14_device,
            a17 = bgk_storage.a17_device,
            jmin = bgk_storage.jmin,
            imin = bgk_storage.imin
        ](sycl::item<2> id){
            const auto j = id.get_id(0) + jmin;
            const auto i = id.get_id(1) + imin;
             if(obs(i,j)==1){
                a01(i,j) = a12(i+1,j-1);
                a03(i,j) = a10(i+1,j+1);
                a05(i,j) = a14(i+1,j  );
                a08(i,j) = a17(i  ,j+1);
                a10(i,j) = a03(i-1,j-1);
                a12(i,j) = a01(i-1,j+1);
                a14(i,j) = a05(i-1,j  );
                a17(i,j) = a08(i  ,j-1);
             }
        });

        #else
        //TODO: port to the new API
        static const size_t range_j = bgk_storage.jmax - bgk_storage.jmin + 1;
        static const size_t range_i = bgk_storage.imax - bgk_storage.imin + 1;
        static const size_t range_j_pow2 = utils::flp2(range_j);
        static const size_t range_i_pow2 = utils::flp2(range_i);
        static const size_t local_x_size = std::min(range_j_pow2, 1024ul); //TODO: 1024 is a magic number
        static const size_t local_y_size = std::min(range_i_pow2, 1024ul); //TODO: 1024 is a magic number
        static const size_t remaining_x = range_j - range_j_pow2;
        static const size_t remaining_y = range_i - range_i_pow2;

	[[maybe_unused]] auto event = q.parallel_for(sycl::nd_range<2>({range_j_pow2, range_i_pow2}, {local_x_size,local_y_size}), [
            obs = bgk_storage.obs_device,
            a01 = bgk_storage.a01_device,
            a03 = bgk_storage.a03_device,
            a05 = bgk_storage.a05_device,
            a08 = bgk_storage.a08_device,
            a10 = bgk_storage.a10_device,
            a12 = bgk_storage.a12_device,
            a14 = bgk_storage.a14_device,
            a17 = bgk_storage.a17_device,
            jmin = bgk_storage.jmin,
            imin = bgk_storage.imin,
	        remaining_x = remaining_x,
	        remaining_y = remaining_y
        ](sycl::nd_item<2> id){
            auto j = id.get_global_id(0) + jmin;
            auto i = id.get_global_id(1) + imin;
             if(obs(i,j)==1){
                a01(i,j) = a12(i+1,j-1);
                a03(i,j) = a10(i+1,j+1);
                a05(i,j) = a14(i+1,j  );
                a08(i,j) = a17(i  ,j+1);
                a10(i,j) = a03(i-1,j-1);
                a12(i,j) = a01(i-1,j+1);
                a14(i,j) = a05(i-1,j  );
                a17(i,j) = a08(i  ,j-1);
             }
	     //Remaining
	     if (j < remaining_x && i < remaining_y){
		j = j + remaining_x;
		i = i + remaining_y;
		if(obs(i,j)==1){
                	a01(i,j) = a12(i+1,j-1);
                	a03(i,j) = a10(i+1,j+1);
                	a05(i,j) = a14(i+1,j  );
                	a08(i,j) = a17(i  ,j+1);
                	a10(i,j) = a03(i-1,j-1);
                	a12(i,j) = a01(i-1,j+1);
                	a14(i,j) = a05(i-1,j  );
                	a17(i,j) = a08(i  ,j-1);
             	}
	     }
        });
        #endif // SYCL_ND_RANGE

        #ifndef SYCL_IN_ORDER_QUEUE
        event.wait_and_throw();
        #endif
    
        // Stop timing
        utils::time(timing.tcountO1);
        utils::system_clock(timing.countO1, timing.count_rate, timing.count_max);
        timing.time_obs = timing.time_obs + static_cast<float>((timing.tcountO1 - timing.tcountO0) / timing.count_rate);
        timing.time_obs1 = timing.time_obs1 + (timing.tcountO1 - timing.tcountO0);

        #ifdef DEBUG_2
            if (bgk_storage.myrank == 0){
                fmt::print("DEBUG2: Exiting from sub. bcond_obs {} {} {} {}\n", bgk_storage.imin, bgk_storage.imax, bgk_storage.jmin, bgk_storage.jmax);
            }
        #endif
    }

}
