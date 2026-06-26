#include <bcond/bcond_periodic.hh>
#include <time.hh>
#include <file_manager.hh>
#include <kernel_utils.hh>
namespace bgk{
    void bcond_periodic(storage& bgk_storage){
        auto& timing = utils::timing::instance();
        utils::system_clock(timing.countA0, timing.count_rate, timing.count_max);
        utils::time(timing.tcountA0);

        auto& q = *bgk_storage.dev_q;

        #ifndef SYCL_ND_RANGE

    [[maybe_unused]] auto event =  q.submit([&](sycl::handler& cgh){    
            constexpr auto start = 0;
            const auto end = bgk_storage.m + 2; // m + 1 (included)
            cgh.parallel_for(sycl::range(end), [
                a01 = bgk_storage.a01_device,
                a03 = bgk_storage.a03_device,
                a05 = bgk_storage.a05_device,
                a08 = bgk_storage.a08_device,
                a10 = bgk_storage.a10_device,
                a12 = bgk_storage.a12_device,
                a14 = bgk_storage.a14_device,
                a17 = bgk_storage.a17_device,
                l = bgk_storage.l,
                l1 = bgk_storage.l1
            ](sycl::item<1> id){
            const auto j = id.get_id(0);
            // front (x = l)
            a01( 0,j) = a01(l,j);
            a03( 0,j) = a03(l,j);
            a05( 0,j) = a05(l,j);
            // rear (x = 0)
            a10(l1,j) = a10(1,j);
            a12(l1,j) = a12(1,j);
            a14(l1,j) = a14(1,j);
            });
        });

        q.submit([&](sycl::handler& cgh){
            #ifndef SYCL_IN_ORDER_QUEUE
            cgh.depends_on(event);
            #endif
            constexpr auto start = 0;
            const auto end = bgk_storage.l + 2; // l + 1 (included)
            cgh.parallel_for(sycl::range(end), [
                a01 = bgk_storage.a01_device,
                a03 = bgk_storage.a03_device,
                a05 = bgk_storage.a05_device,
                a08 = bgk_storage.a08_device,
                a10 = bgk_storage.a10_device,
                a12 = bgk_storage.a12_device,
                a14 = bgk_storage.a14_device,
                a17 = bgk_storage.a17_device,
                m = bgk_storage.m,
                m1 = bgk_storage.m1
            ](sycl::item<1> id){
                const auto i = id.get_id(0);
            // left (y = 0)  
           a17(i,m1) = a17(i,1);
           a01(i,m1) = a01(i,1);
           a10(i,m1) = a10(i,1);

            // right (y = m) 
           a03(i,0) = a03(i,m);
           a08(i,0) = a08(i,m);
           a12(i,0) = a12(i,m);
            });
        });

        #else
        static kernel_size_t bcond_periodic_j_setup(bgk_storage, bgk_storage.m + 2);
    [[maybe_unused]] auto event =  q.submit([&](sycl::handler& cgh){    
            cgh.parallel_for<class bcond_periodic_j>(sycl::nd_range(
                {bcond_periodic_j_setup.sizes.grid_size},
                {bcond_periodic_j_setup.sizes.block_size}), 
                [
                a01 = bgk_storage.a01_device,
                a03 = bgk_storage.a03_device,
                a05 = bgk_storage.a05_device,
                a08 = bgk_storage.a08_device,
                a10 = bgk_storage.a10_device,
                a12 = bgk_storage.a12_device,
                a14 = bgk_storage.a14_device,
                a17 = bgk_storage.a17_device,
                l = bgk_storage.l,
                l1 = bgk_storage.l1,
                m = bgk_storage.m,
                remaining = bcond_periodic_j_setup.sizes.remaining
            ](sycl::nd_item<1> id){
            auto j = id.get_global_id(0);
            // front (x = l)
            a01( 0,j) = a01(l,j);
            a03( 0,j) = a03(l,j);
            a05( 0,j) = a05(l,j);
            // rear (x = 0)
            a10(l1,j) = a10(1,j);
            a12(l1,j) = a12(1,j);
            a14(l1,j) = a14(1,j);
            
            if (j < remaining){
                j = j + m;
                // front (x = l)
                a01( 0,j) = a01(l,j);
                a03( 0,j) = a03(l,j);
                a05( 0,j) = a05(l,j);
                // rear (x = 0)
                a10(l1,j) = a10(1,j);
                a12(l1,j) = a12(1,j);
                a14(l1,j) = a14(1,j);
            }
            });
        });

        static kernel_size_t bcond_periodic_i_setup(bgk_storage, bgk_storage.l + 2);
        q.submit([&](sycl::handler& cgh){
            #ifndef SYCL_IN_ORDER_QUEUE
cgh.depends_on(event);
#endif
            cgh.parallel_for<class bcond_periodic_i>(sycl::nd_range{
                {bcond_periodic_i_setup.sizes.grid_size},
                {bcond_periodic_i_setup.sizes.block_size}
            }, [
                a01 = bgk_storage.a01_device,
                a03 = bgk_storage.a03_device,
                a05 = bgk_storage.a05_device,
                a08 = bgk_storage.a08_device,
                a10 = bgk_storage.a10_device,
                a12 = bgk_storage.a12_device,
                a14 = bgk_storage.a14_device,
                a17 = bgk_storage.a17_device,
                m = bgk_storage.m,
                m1 = bgk_storage.m1,
                l = bgk_storage.l,
                remaining = bcond_periodic_i_setup.sizes.remaining
            ](sycl::nd_item<1> id){
                auto i = id.get_global_id(0);
            // left (y = 0)  
           a17(i,m1) = a17(i,1);
           a01(i,m1) = a01(i,1);
           a10(i,m1) = a10(i,1);

            // right (y = m) 
           a03(i,0) = a03(i,m);
           a08(i,0) = a08(i,m);
           a12(i,0) = a12(i,m);

            if (i < remaining){
                i = i + l;
                // left (y = 0)  
                a17(i,m1) = a17(i,1);
                a01(i,m1) = a01(i,1);
                a10(i,m1) = a10(i,1);

                // right (y = m) 
                a03(i,0) = a03(i,m);
                a08(i,0) = a08(i,m);
                a12(i,0) = a12(i,m);
            }

            });
        });

        #endif

        #ifndef SYCL_IN_ORDER_QUEUE
        event.wait_and_throw();
        #endif

        // Stop timing
        utils::time(timing.tcountA1);
        utils::system_clock(timing.countA1, timing.count_rate, timing.count_max);
        timing.time_bc = timing.time_bc + static_cast<float>((timing.tcountA1 - timing.tcountA0) / timing.count_rate);
        timing.time_bc1 = timing.time_bc1 + (timing.tcountA1 - timing.tcountA0);

        #ifdef DEBUG_2
            if (bgk_storage.myrank == 0){
                fmt::print("DEBUG2: Exiting from sub. bcond_periodic\n");
            }
        #endif
    }
}