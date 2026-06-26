
#include <col_MC.hh>
#include <fstream>
#include <iomanip>
#include <time.hh>
#include <kernel_utils.hh>
#include <fmt/core.h>

namespace bgk {
void col_MC(storage &bgk_storage, const int itime) {
    auto &q = *bgk_storage.dev_q;
    auto &timing = utils::timing::instance();
    constexpr real_kinds::mykind pi = 3.14159265358979;

#ifdef FUSED

    #ifdef DEBUG_3
    real_kinds::mykind cte;
    std::ostringstream file_nameD;
    file_nameD << "debug." << std::setw(3) << std::setfill('0') << itime << "." << std::setw(3) << std::setfill('0')
               << bgk_storage.myrank << ".log";
    std::ofstream debug_file(file_nameD.str()); // debug file

    // call probe(itime, (3*l/4), (m/2));
    #endif

    #ifdef NOSHIFT
    static constexpr real_kinds::mykind cte1 = storage::zero;
    #else
    static constexpr real_kinds::mykind cte1 = storage::uno;
    #endif
    static constexpr real_kinds::mykind cte0 = storage::uno - cte1;

#ifndef SYCL_ND_RANGE
    // Porting kernels as range parallel_for, gonna switch to nd_range in optimization phase
    auto event = q.parallel_for<class col_MC>(sycl::range(bgk_storage.m, bgk_storage.l),
         [a01 = bgk_storage.a01_device, a03 = bgk_storage.a03_device, a05 = bgk_storage.a05_device,
             a08 = bgk_storage.a08_device, a10 = bgk_storage.a10_device, a12 = bgk_storage.a12_device,
             a14 = bgk_storage.a14_device, a17 = bgk_storage.a17_device, a19 = bgk_storage.a19_device,
             b01 = bgk_storage.b01_device, b03 = bgk_storage.b03_device, b05 = bgk_storage.b05_device,
             b08 = bgk_storage.b08_device, b10 = bgk_storage.b10_device, b12 = bgk_storage.b12_device,
             b14 = bgk_storage.b14_device, b17 = bgk_storage.b17_device, b19 = bgk_storage.b19_device,
             obs = bgk_storage.obs_device, omega1 = bgk_storage.omega1,
             cteS = bgk_storage.cteS,
             omega = bgk_storage.omega,
             fgrad = bgk_storage.fgrad](sycl::item<2> id) {

             const auto j = id.get_id(0) + 1;
             const auto i = id.get_id(1) + 1;
             real_kinds::mykind x01, x03, x05, x08, x10;
             real_kinds::mykind x12, x14, x17, x19;
             real_kinds::mykind e01, e03, e05, e08, e10;
             real_kinds::mykind e12, e14, e17, e19;
             real_kinds::mykind n01, n03, n05, n08, n10;
             real_kinds::mykind n12, n14, n17, n19;
             real_kinds::mykind rho, rhoinv, vx, vy, vx2, vy2, vsq;
             real_kinds::mykind vxpy, vxmy, rp1, rp2, rp0;
             real_kinds::mykind qxpy, qxmy, qx, qy, q0;
             real_kinds::mykind Pxx, Pxy, Pyx, Pyy, Ptotal;
             real_kinds::mykind Ts;


             x01 = a01(i - 1, j + 1);
             x03 = a03(i - 1, j - 1);
             x05 = a05(i - 1, j);
             x08 = a08(i, j - 1);
             x10 = a10(i + 1, j + 1);
             x12 = a12(i + 1, j - 1);
             x14 = a14(i + 1, j);
             x17 = a17(i, j + 1);
             x19 = a19(i, j);

             // natural way of expressing rho
             // rho = (x01 + x03 + x05 + x08 + x10 + x12 + x14 + x17 + x19) + cte1;

             // better for half precision
             rho = (((x01 + x03 + x10 + x12) + (x05 + x08 + x14 + x17)) + x19) + cte1;

             rhoinv = 1.0 / rho;

             vx = ((x03 - x12) + (x01 - x10) + (x05 - x14)) * rhoinv;
             vy = ((x03 - x01) + (x12 - x10) + (x08 - x17)) * rhoinv;

             // This debug call is not portable to SYCL, not to a file at least
             //             # ifdef DEBUG_3
             //         write(41,3131) i+offset(1),j+offset(2), obs(i,j),   &
             //                         vx,vy,rho
             // !                       x01,x03,x05,x08,x10,x12,x14,x17,x19
             // # endif

             // Quadratic terms
             vx2 = vx * vx;
             vy2 = vy * vy;

             vsq = vx2 + vy2;

             vxpy = vx + vy;
             vxmy = vx - vy;

             qxpy = cte0 + storage::qf * (storage::tre * vxpy * vxpy - vsq);
             qxmy = cte0 + storage::qf * (storage::tre * vxmy * vxmy - vsq);
             qx = cte0 + storage::qf * (storage::tre * vx2 - vsq);
             qy = cte0 + storage::qf * (storage::tre * vy2 - vsq);
             q0 = cte0 + storage::qf * (-vsq);

             // Linear terms
             vx = storage::rf * vx;
             vy = storage::rf * vy;
             vxpy = storage::rf * vxpy;
             vxmy = storage::rf * vxmy;

             // Constant terms
             rp0 = rho * storage::p0;
             rp1 = rho * storage::p1;
             rp2 = rho * storage::p2;

             // Equilibrium distribution
             e01 = rp2 * (+vxmy + qxmy) + cte1 * (rp2 - storage::p2);
             e03 = rp2 * (+vxpy + qxpy) + cte1 * (rp2 - storage::p2);
             e05 = rp1 * (+vx + qx) + cte1 * (rp1 - storage::p1);
             e08 = rp1 * (+vy + qy) + cte1 * (rp1 - storage::p1);
             e10 = rp2 * (-vxpy + qxpy) + cte1 * (rp2 - storage::p2);
             e12 = rp2 * (-vxmy + qxmy) + cte1 * (rp2 - storage::p2);
             e14 = rp1 * (-vx + qx) + cte1 * (rp1 - storage::p1);
             e17 = rp1 * (-vy + qy) + cte1 * (rp1 - storage::p1);
             e19 = rp0 * (+q0) + cte1 * (rp0 - storage::p0);
             
             #ifdef LES
             // compute les

             // non-equilibrium distribution
             n01 = x01 - e01;
             n03 = x03 - e03;
             n05 = x05 - e05;
             n08 = x08 - e08;
             n10 = x10 - e10;
             n12 = x12 - e12;
             n14 = x14 - e14;
             n17 = x17 - e17;

             // compute Pij
             // Pxx = cx(01)*cx(01)*n01 + &
             //       cx(03)*cx(03)*n03 + &
             //       cx(05)*cx(05)*n05 + &
             //       cx(08)*cx(08)*n08 + &
             //       cx(10)*cx(10)*n10 + &
             //       cx(12)*cx(12)*n12 + &
             //       cx(14)*cx(14)*n14 + &
             //       cx(17)*cx(17)*n17

             Pxx = n01 + n03 + n05 + n10 + n12 + n14;

             // Pyy = cy(01)*cy(01)*n01 + &
             //       cy(03)*cy(03)*n03 + &
             //       cy(05)*cy(05)*n05 + &
             //       cy(08)*cy(08)*n08 + &
             //       cy(10)*cy(10)*n10 + &
             //       cy(12)*cy(12)*n12 + &
             //       cy(14)*cy(14)*n14 + &
             //       cy(17)*cy(17)*n17

             Pyy = n01 + n03 + n08 + n10 + n12 + n17;

             // Pxy = cx(01)*cy(01)*n01 + &
             //       cx(03)*cy(03)*n03 + &
             //       cx(05)*cy(05)*n05 + &
             //       cx(08)*cy(08)*n08 + &
             //       cx(10)*cy(10)*n10 + &
             //       cx(12)*cy(12)*n12 + &
             //       cx(14)*cy(14)*n14 + &
             //       cx(17)*cy(17)*n17

             Pxy = -n01 + n03 + n10 - n12;

             Pyx = Pxy;

             // calculate Pi total
             Ptotal = sycl::sqrt((Pxx * Pxx) + (2 * Pxy * Pyx) + (Pyy * Pyy));

             // adding turbulent viscosity
             Ts = 1 / (2 * omega1) + sqrt(18.0 * (cteS * cteS) * Ptotal + (1.0 / omega1) * (1.0 / omega1)) / 2;
             omega = 1 / Ts; // TODO: CHeck if the value is ever used in other kernels

        #ifdef QQQQQQ
             if(itime % 1000 == 0) {
                 if((i == l / 2) && (j == m / 2)) {
                     // omega = 2.0 / (6.0 * svisc + 1.0)
                     std::cout << itime << " " << (2 / omega1 - 1) / 6 << " " << (2 / omega - 1) / 6 << "\n";
                     printf("%d %f %f\n", itime, (2 / omega1 - 1) / 6, (2 / omega - 1) / 6);
                 }
             }
        #endif

    #endif // LES
    
    // forcing term

    // initialize constant.....
    // Mumbo-jambo to try to keep constexpr variables
    #if !defined(CHANNEL) && !defined(MYVERSION)
             static constexpr real_kinds::mykind forcex = storage::zero;
             static constexpr real_kinds::mykind forcey = storage::zero;
    #endif

    #ifdef CHANNEL
        #ifdef FORCING_Y
             real_kinds::mykind forcey = fgrad * rho;
             static constexpr real_kinds::mykind forcex = storage::zero;

        #else
            real_kinds::mykind forcex = fgrad * rho;  // default value...
            static constexpr real_kinds::mykind forcey = storage::zero;
        #endif
    #endif

    #ifdef MYVERSION
        #ifdef FORCING_Y
             real_kinds::mykind forcey = fgrad * rho;
             static constexpr real_kinds::mykind forcex = storage::zero;

        #else
            real_kinds::mykind forcex = fgrad * rho;  // default value...
            static constexpr real_kinds::mykind forcey = storage::zero;
        #endif
    #endif

             // loop on populations
             b01(i, j) = x01 - omega * (x01 - e01) + (+forcex - forcey);
             b03(i, j) = x03 - omega * (x03 - e03) + (+forcex + forcey);
             b05(i, j) = x05 - omega * (x05 - e05) + (+forcex);
             b08(i, j) = x08 - omega * (x08 - e08) + (+forcey);
             b10(i, j) = x10 - omega * (x10 - e10) + (-forcex - forcey);
             b12(i, j) = x12 - omega * (x12 - e12) + (-forcex + forcey);
             b14(i, j) = x14 - omega * (x14 - e14) + (-forcex);
             b17(i, j) = x17 - omega * (x17 - e17) + (-forcey);
             a19(i, j) = x19 - omega * (x19 - e19);
         });
#else
    // NDRange version
    //TODO: should be substituted with a better solution. Also I don't check for remaining elements
    static kernel_size_t col_mc_setup(bgk_storage, bgk_storage.m * bgk_storage.l); 
    
    const sycl::range<2> global_range(bgk_storage.m, bgk_storage.l);

    #if defined(BGK_SYCL_LAYOUT_LEFT)
    const sycl::range<2> local_range(1,col_mc_setup.sizes.block_size); 
    #elif defined(BGK_SYCL_LAYOUT_RIGHT)
    const sycl::range<2> local_range(col_mc_setup.sizes.block_size,1);
    #else
    const sycl::range<2> local_range(1, col_mc_setup.sizes.block_size); //TODO: REMOVE, THE GRID COULD BE SMALLER IN OTHER USE CASES
    #endif
    
    const sycl::nd_range<2> nd_range(global_range, local_range);
    
    auto event = q.submit([&](sycl::handler& cgh){
	// sycl::stream ss{static_cast<size_t>(1024), 1024, cgh};
    cgh.parallel_for<class col_MC>(nd_range,
         [a01 = bgk_storage.a01_device, a03 = bgk_storage.a03_device, a05 = bgk_storage.a05_device,
             a08 = bgk_storage.a08_device, a10 = bgk_storage.a10_device, a12 = bgk_storage.a12_device,
             a14 = bgk_storage.a14_device, a17 = bgk_storage.a17_device, a19 = bgk_storage.a19_device,
             b01 = bgk_storage.b01_device, b03 = bgk_storage.b03_device, b05 = bgk_storage.b05_device,
             b08 = bgk_storage.b08_device, b10 = bgk_storage.b10_device, b12 = bgk_storage.b12_device,
             b14 = bgk_storage.b14_device, b17 = bgk_storage.b17_device, b19 = bgk_storage.b19_device,
             obs = bgk_storage.obs_device, omega1 = bgk_storage.omega1,
             cteS = bgk_storage.cteS,
             omega = bgk_storage.omega,
	    //  ss,
             fgrad = bgk_storage.fgrad](sycl::nd_item<2> id) {

	         #if defined(BGK_SYCL_LAYOUT_LEFT)
	    	 const auto j = id.get_global_id(0) + 1;
                 const auto i = id.get_global_id(1) + 1;  
		 #elif defined(BGK_SYCL_LAYOUT_RIGHT)
 		const auto j = id.get_global_id(1) + 1;
                 const auto i = id.get_global_id(0) + 1;	
		#else
            const auto j = id.get_global_id(0) + 1;
            const auto i = id.get_global_id(1) + 1; 
		 #endif
             real_kinds::mykind x01, x03, x05, x08, x10;
             real_kinds::mykind x12, x14, x17, x19;
             real_kinds::mykind e01, e03, e05, e08, e10;
             real_kinds::mykind e12, e14, e17, e19;
             real_kinds::mykind n01, n03, n05, n08, n10;
             real_kinds::mykind n12, n14, n17, n19;
             real_kinds::mykind rho, rhoinv, vx, vy, vx2, vy2, vsq;
             real_kinds::mykind vxpy, vxmy, rp1, rp2, rp0;
             real_kinds::mykind qxpy, qxmy, qx, qy, q0;
             real_kinds::mykind Pxx, Pxy, Pyx, Pyy, Ptotal;
             real_kinds::mykind Ts;

	    //  if (i == 0 && j == 0){
		// ss << id.get_local_range(0) << "," << id.get_local_range(1) << "\n";
	    //  }
             x01 = a01(i - 1, j + 1);
             x03 = a03(i - 1, j - 1);
             x05 = a05(i - 1, j);
             x08 = a08(i, j - 1);
             x10 = a10(i + 1, j + 1);
             x12 = a12(i + 1, j - 1);
             x14 = a14(i + 1, j);
             x17 = a17(i, j + 1);
             x19 = a19(i, j);

             // natural way of expressing rho
             // rho = (x01 + x03 + x05 + x08 + x10 + x12 + x14 + x17 + x19) + cte1;

             // better for half precision
             rho = (((x01 + x03 + x10 + x12) + (x05 + x08 + x14 + x17)) + x19) + cte1;

             rhoinv = 1.0 / rho;

             vx = ((x03 - x12) + (x01 - x10) + (x05 - x14)) * rhoinv;
             vy = ((x03 - x01) + (x12 - x10) + (x08 - x17)) * rhoinv;

             // This debug call is not portable to SYCL, not to a file at least
             //             # ifdef DEBUG_3
             //         write(41,3131) i+offset(1),j+offset(2), obs(i,j),   &
             //                         vx,vy,rho
             // !                       x01,x03,x05,x08,x10,x12,x14,x17,x19
             // # endif

             // Quadratic terms
             vx2 = vx * vx;
             vy2 = vy * vy;

             vsq = vx2 + vy2;

             vxpy = vx + vy;
             vxmy = vx - vy;

             qxpy = cte0 + storage::qf * (storage::tre * vxpy * vxpy - vsq);
             qxmy = cte0 + storage::qf * (storage::tre * vxmy * vxmy - vsq);
             qx = cte0 + storage::qf * (storage::tre * vx2 - vsq);
             qy = cte0 + storage::qf * (storage::tre * vy2 - vsq);
             q0 = cte0 + storage::qf * (-vsq);

             // Linear terms
             vx = storage::rf * vx;
             vy = storage::rf * vy;
             vxpy = storage::rf * vxpy;
             vxmy = storage::rf * vxmy;

             // Constant terms
             rp0 = rho * storage::p0;
             rp1 = rho * storage::p1;
             rp2 = rho * storage::p2;

             // Equilibrium distribution
             e01 = rp2 * (+vxmy + qxmy) + cte1 * (rp2 - storage::p2);
             e03 = rp2 * (+vxpy + qxpy) + cte1 * (rp2 - storage::p2);
             e05 = rp1 * (+vx + qx) + cte1 * (rp1 - storage::p1);
             e08 = rp1 * (+vy + qy) + cte1 * (rp1 - storage::p1);
             e10 = rp2 * (-vxpy + qxpy) + cte1 * (rp2 - storage::p2);
             e12 = rp2 * (-vxmy + qxmy) + cte1 * (rp2 - storage::p2);
             e14 = rp1 * (-vx + qx) + cte1 * (rp1 - storage::p1);
             e17 = rp1 * (-vy + qy) + cte1 * (rp1 - storage::p1);
             e19 = rp0 * (+q0) + cte1 * (rp0 - storage::p0);
             
             #ifdef LES
             // compute les

             // non-equilibrium distribution
             n01 = x01 - e01;
             n03 = x03 - e03;
             n05 = x05 - e05;
             n08 = x08 - e08;
             n10 = x10 - e10;
             n12 = x12 - e12;
             n14 = x14 - e14;
             n17 = x17 - e17;

             // compute Pij
             // Pxx = cx(01)*cx(01)*n01 + &
             //       cx(03)*cx(03)*n03 + &
             //       cx(05)*cx(05)*n05 + &
             //       cx(08)*cx(08)*n08 + &
             //       cx(10)*cx(10)*n10 + &
             //       cx(12)*cx(12)*n12 + &
             //       cx(14)*cx(14)*n14 + &
             //       cx(17)*cx(17)*n17

             Pxx = n01 + n03 + n05 + n10 + n12 + n14;

             // Pyy = cy(01)*cy(01)*n01 + &
             //       cy(03)*cy(03)*n03 + &
             //       cy(05)*cy(05)*n05 + &
             //       cy(08)*cy(08)*n08 + &
             //       cy(10)*cy(10)*n10 + &
             //       cy(12)*cy(12)*n12 + &
             //       cy(14)*cy(14)*n14 + &
             //       cy(17)*cy(17)*n17

             Pyy = n01 + n03 + n08 + n10 + n12 + n17;

             // Pxy = cx(01)*cy(01)*n01 + &
             //       cx(03)*cy(03)*n03 + &
             //       cx(05)*cy(05)*n05 + &
             //       cx(08)*cy(08)*n08 + &
             //       cx(10)*cy(10)*n10 + &
             //       cx(12)*cy(12)*n12 + &
             //       cx(14)*cy(14)*n14 + &
             //       cx(17)*cy(17)*n17

             Pxy = -n01 + n03 + n10 - n12;

             Pyx = Pxy;

             // calculate Pi total
             Ptotal = sycl::sqrt((Pxx * Pxx) + (2 * Pxy * Pyx) + (Pyy * Pyy));

             // adding turbulent viscosity
             Ts = 1 / (2 * omega1) + sqrt(18.0 * (cteS * cteS) * Ptotal + (1.0 / omega1) * (1.0 / omega1)) / 2;
             omega = 1 / Ts; // TODO: CHeck if the value is ever used in other kernels

        #ifdef QQQQQQ
             if(itime % 1000 == 0) {
                 if((i == l / 2) && (j == m / 2)) {
                     // omega = 2.0 / (6.0 * svisc + 1.0)
                     std::cout << itime << " " << (2 / omega1 - 1) / 6 << " " << (2 / omega - 1) / 6 << "\n";
                     printf("%d %f %f\n", itime, (2 / omega1 - 1) / 6, (2 / omega - 1) / 6);
                 }
             }
        #endif

    #endif // LES
    
    // forcing term

    // initialize constant.....
    // Mumbo-jambo to try to keep constexpr variables
    #if !defined(CHANNEL) && !defined(MYVERSION)
             static constexpr real_kinds::mykind forcex = storage::zero;
             static constexpr real_kinds::mykind forcey = storage::zero;
    #endif

    #ifdef CHANNEL
        #ifdef FORCING_Y
             real_kinds::mykind forcey = fgrad * rho;
             static constexpr real_kinds::mykind forcex = storage::zero;

        #else
            real_kinds::mykind forcex = fgrad * rho;  // default value...
            static constexpr real_kinds::mykind forcey = storage::zero;
        #endif
    #endif

    #ifdef MYVERSION
        #ifdef FORCING_Y
             real_kinds::mykind forcey = fgrad * rho;
             static constexpr real_kinds::mykind forcex = storage::zero;

        #else
            real_kinds::mykind forcex = fgrad * rho;  // default value...
            static constexpr real_kinds::mykind forcey = storage::zero;
        #endif
    #endif

             // loop on populations
             b01(i, j) = x01 - omega * (x01 - e01) + (+forcex - forcey);
             b03(i, j) = x03 - omega * (x03 - e03) + (+forcex + forcey);
             b05(i, j) = x05 - omega * (x05 - e05) + (+forcex);
             b08(i, j) = x08 - omega * (x08 - e08) + (+forcey);
             b10(i, j) = x10 - omega * (x10 - e10) + (-forcex - forcey);
             b12(i, j) = x12 - omega * (x12 - e12) + (-forcex + forcey);
             b14(i, j) = x14 - omega * (x14 - e14) + (-forcex);
             b17(i, j) = x17 - omega * (x17 - e17) + (-forcey);
             a19(i, j) = x19 - omega * (x19 - e19);
         });
    });
#endif // SYCL_ND_RANGE

    event.wait_and_throw();
    // Swap populations
    bgk_storage.swap_populations();
#endif // FUSED


#ifdef DEBUG_2
    if(bgk_storage.myrank == 0) std::cout << "DEBUG2: Exiting from sub. coll_MC\n";
#endif
}
} // namespace bgk
