#include <bcond/bcond_inflow.hh>
#include <file_manager.hh>
#include <time.hh>
#include <kernel_utils.hh>

namespace bgk {
void bcond_inflow(storage &bgk_storage) {
   
    real_kinds::mykind cte1;
    auto& timing = utils::timing::instance();
#ifdef NOSHIFT
    cte1 = storage::uno;
#else
    cte1 = storage::zero;
#endif

    utils::system_clock(timing.countA0, timing.count_rate, timing.count_max);
    utils::time(timing.tcountA0);
    auto q = *bgk_storage.dev_q;

    bgk_storage.u_inflow = 0.1; //TODO: comment in the old code says to remove this
    #ifndef SYCL_ND_RANGE
    // loop fused for perfomance reason (on GPU)        
    // front, outflow  (x = l)
    [[maybe_unused]] const auto event = q.submit([&](sycl::handler& cgh){
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
            a19 = bgk_storage.a19_device,
            u_inflow = bgk_storage.u_inflow,
            l = bgk_storage.l,
            l1 = bgk_storage.l1,
            cte1
        ](sycl::item<1> id){
            const auto j = id.get_linear_id();
            real_kinds::mykind xj,yj;
            real_kinds::mykind cvsq,crho, rhoinv;
            real_kinds::mykind cx01,cx03,cx05;
            real_kinds::mykind cx10,cx12,cx14;
        // y = (real(y,mykind) -0.5 - 0.5*real(m,mykind))/(0.5*real(m,mykind))
           crho  = storage::uno;
           rhoinv= storage::uno;
        // front, outflow  (x = l)
           xj = ((a03(l,j)-a12(l,j))+(a01(l,j)-a10(l,j))+(a05(l,j)-a14(l,j)))*rhoinv;
           yj = ((a03(l,j)-a01(l,j))+(a12(l,j)-a10(l,j))+(a08(l,j)-a17(l,j)))*rhoinv;

           cvsq=xj*xj+yj*yj;

           cx10 = storage::rf*(-xj-yj)+storage::qf*(3.0*(xj+yj)*(xj+yj)-cvsq);
           cx12 = storage::rf*(-xj+yj)+storage::qf*(3.0*(xj-yj)*(xj-yj)-cvsq);
           cx14 = storage::rf*(-xj   )+storage::qf*(3.0*(xj   )*(xj   )-cvsq);

           a10(l1,j) = crho*storage::p2*(cte1+cx10);
           a12(l1,j) = crho*storage::p2*(cte1+cx12);
           a14(l1,j) = crho*storage::p1*(cte1+cx14);

         // rear, inflow (x = 0)
           xj = u_inflow;
           yj = storage::zero;

           cvsq=xj*xj+yj*yj;

           cx01 = storage::rf*( xj-yj   )+storage::qf*(3.0*(xj-yj)*(xj-yj)-cvsq);
           cx03 = storage::rf*( xj+yj   )+storage::qf*(3.0*(xj+yj)*(xj+yj)-cvsq);
           cx05 = storage::rf*( xj      )+storage::qf*(3.0*(xj   )*(xj   )-cvsq);

           a01(0,j) = crho*storage::p2*(cte1+cx01);
           a03(0,j) = crho*storage::p2*(cte1+cx03);
           a05(0,j) = crho*storage::p1*(cte1+cx05);
        });
    });

// ----------------------------------------------
// left (y = 0)  
// right (y = m) 
// ----------------------------------------------
// 

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
            a19 = bgk_storage.a19_device,
            u_inflow = bgk_storage.u_inflow,
            m = bgk_storage.m,
            m1 = bgk_storage.m1,
            cte1
        ](sycl::item<1> id){
            const auto i = id.get_linear_id();
            // left, noslip  (y = 0)  
           a17(i,m1) = a17(i,1);
           a01(i,m1) = a01(i,1);
           a10(i,m1) = a10(i,1);
            // right, noslip  (y = m) 
           a03(i,0) = a03(i,m);
           a08(i,0) = a08(i,m);
           a12(i,0) = a12(i,m);
        });
});

#else
    
    static kernel_size_t bcond_inflo_front_setup(bgk_storage, bgk_storage.m + 2);

    // loop fused for perfomance reason (on GPU)        
    // front, outflow  (x = l)
    [[maybe_unused]] const auto event = q.submit([&](sycl::handler& cgh){

        cgh.parallel_for<class bcond_inflow_front>(sycl::nd_range({bcond_inflo_front_setup.sizes.grid_size}, 
        {bcond_inflo_front_setup.sizes.block_size}), [
            a01 = bgk_storage.a01_device,
            a03 = bgk_storage.a03_device,
            a05 = bgk_storage.a05_device,
            a08 = bgk_storage.a08_device,
            a10 = bgk_storage.a10_device,
            a12 = bgk_storage.a12_device,
            a14 = bgk_storage.a14_device,
            a17 = bgk_storage.a17_device,
            a19 = bgk_storage.a19_device,
            u_inflow = bgk_storage.u_inflow,
            l = bgk_storage.l,
            l1 = bgk_storage.l1,
            cte1,
            m = bgk_storage.m,
            remaining = bcond_inflo_front_setup.sizes.remaining
        ](sycl::nd_item<1> id){
            auto j = id.get_global_id(0);
            real_kinds::mykind xj,yj;
            real_kinds::mykind cvsq,crho, rhoinv;
            real_kinds::mykind cx01,cx03,cx05;
            real_kinds::mykind cx10,cx12,cx14;
        // y = (real(y,mykind) -0.5 - 0.5*real(m,mykind))/(0.5*real(m,mykind))
           crho  = storage::uno;
           rhoinv= storage::uno;
        // front, outflow  (x = l)
           xj = ((a03(l,j)-a12(l,j))+(a01(l,j)-a10(l,j))+(a05(l,j)-a14(l,j)))*rhoinv;
           yj = ((a03(l,j)-a01(l,j))+(a12(l,j)-a10(l,j))+(a08(l,j)-a17(l,j)))*rhoinv;

           cvsq=xj*xj+yj*yj;

           cx10 = storage::rf*(-xj-yj)+storage::qf*(3.0*(xj+yj)*(xj+yj)-cvsq);
           cx12 = storage::rf*(-xj+yj)+storage::qf*(3.0*(xj-yj)*(xj-yj)-cvsq);
           cx14 = storage::rf*(-xj   )+storage::qf*(3.0*(xj   )*(xj   )-cvsq);

           a10(l1,j) = crho*storage::p2*(cte1+cx10);
           a12(l1,j) = crho*storage::p2*(cte1+cx12);
           a14(l1,j) = crho*storage::p1*(cte1+cx14);

         // rear, inflow (x = 0)
           xj = u_inflow;
           yj = storage::zero;

           cvsq=xj*xj+yj*yj;

           cx01 = storage::rf*( xj-yj   )+storage::qf*(3.0*(xj-yj)*(xj-yj)-cvsq);
           cx03 = storage::rf*( xj+yj   )+storage::qf*(3.0*(xj+yj)*(xj+yj)-cvsq);
           cx05 = storage::rf*( xj      )+storage::qf*(3.0*(xj   )*(xj   )-cvsq);

           a01(0,j) = crho*storage::p2*(cte1+cx01);
           a03(0,j) = crho*storage::p2*(cte1+cx03);
           a05(0,j) = crho*storage::p1*(cte1+cx05);

	   //Remaining
	   if ( j < remaining ) //There are two remaining
	   {
		    j = j + m;
	       crho  = storage::uno;
           rhoinv= storage::uno;
        // front, outflow  (x = l)
           xj = ((a03(l,j)-a12(l,j))+(a01(l,j)-a10(l,j))+(a05(l,j)-a14(l,j)))*rhoinv;
           yj = ((a03(l,j)-a01(l,j))+(a12(l,j)-a10(l,j))+(a08(l,j)-a17(l,j)))*rhoinv;

           cvsq=xj*xj+yj*yj;

           cx10 = storage::rf*(-xj-yj)+storage::qf*(3.0*(xj+yj)*(xj+yj)-cvsq);
           cx12 = storage::rf*(-xj+yj)+storage::qf*(3.0*(xj-yj)*(xj-yj)-cvsq);
           cx14 = storage::rf*(-xj   )+storage::qf*(3.0*(xj   )*(xj   )-cvsq);

           a10(l1,j) = crho*storage::p2*(cte1+cx10);
           a12(l1,j) = crho*storage::p2*(cte1+cx12);
           a14(l1,j) = crho*storage::p1*(cte1+cx14);

         // rear, inflow (x = 0)
           xj = u_inflow;
           yj = storage::zero;

           cvsq=xj*xj+yj*yj;

           cx01 = storage::rf*( xj-yj   )+storage::qf*(3.0*(xj-yj)*(xj-yj)-cvsq);
           cx03 = storage::rf*( xj+yj   )+storage::qf*(3.0*(xj+yj)*(xj+yj)-cvsq);
           cx05 = storage::rf*( xj      )+storage::qf*(3.0*(xj   )*(xj   )-cvsq);

           a01(0,j) = crho*storage::p2*(cte1+cx01);
           a03(0,j) = crho*storage::p2*(cte1+cx03);
           a05(0,j) = crho*storage::p1*(cte1+cx05);
	   }

        });
    });

// ----------------------------------------------
// left (y = 0)  
// right (y = m) 
// ----------------------------------------------
// 

static kernel_size_t bcond_inflo_left_right_setup(bgk_storage, bgk_storage.l + 2);
q.submit([&](sycl::handler& cgh){
        #ifndef SYCL_IN_ORDER_QUEUE
cgh.depends_on(event);
#endif
	cgh.parallel_for<class bcond_inflow_left_right>(sycl::nd_range({bcond_inflo_left_right_setup.sizes.grid_size}, {bcond_inflo_left_right_setup.sizes.block_size}), [
            a01 = bgk_storage.a01_device,
            a03 = bgk_storage.a03_device,
            a05 = bgk_storage.a05_device,
            a08 = bgk_storage.a08_device,
            a10 = bgk_storage.a10_device,
            a12 = bgk_storage.a12_device,
            a14 = bgk_storage.a14_device,
            a17 = bgk_storage.a17_device,
            a19 = bgk_storage.a19_device,
            u_inflow = bgk_storage.u_inflow,
            m = bgk_storage.m,
            m1 = bgk_storage.m1,
            cte1,
            l = bgk_storage.l,
            remaining = bcond_inflo_left_right_setup.sizes.remaining
        ](sycl::nd_item<1> id){
            auto i = id.get_global_id(0);
            // left, noslip  (y = 0)  
           a17(i,m1) = a17(i,1);
           a01(i,m1) = a01(i,1);
           a10(i,m1) = a10(i,1);
            // right, noslip  (y = m) 
           a03(i,0) = a03(i,m);
           a08(i,0) = a08(i,m);
           a12(i,0) = a12(i,m);

	   if (i < remaining) // There are two remeaining
	   {
		i = i + l;
		// left, noslip  (y = 0)  
           	a17(i,m1) = a17(i,1);
           	a01(i,m1) = a01(i,1);
           	a10(i,m1) = a10(i,1);
            	// right, noslip  (y = m) 
           	a03(i,0) = a03(i,m);
           	a08(i,0) = a08(i,m);
           	a12(i,0) = a12(i,m);       
	   }
	   });
});
#endif

#ifndef SYCL_IN_ORDER_QUEUE
q.wait_and_throw();
#endif

// Stop timing
utils::time(timing.tcountA1);
utils::system_clock(timing.countA1, timing.count_rate, timing.count_max);
timing.time_bc = timing.time_bc + static_cast<float>((timing.tcountA1 - timing.tcountA0) / timing.count_rate);
timing.time_bc1 = timing.time_bc1 + (timing.tcountA1 - timing.tcountA0);

#ifdef DEBUG_2 
    if (bgk_storage.myrank == 0){
        fmt::print("DEBUG2: Exiting from sub. bcond_inflow\n");
    }
#endif

}
} // namespace bgk
