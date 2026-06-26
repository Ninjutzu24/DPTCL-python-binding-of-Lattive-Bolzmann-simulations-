#include <fmt/core.h>
#include <iostream>
#include <check_case.hh>
#include <utils.hh>
#include <file_manager.hh>

namespace bgk {
void git_info() {
    //TODO: implement git_info
}
/*...*/

}

namespace bgk {
/*...*/
void check_case(const storage &bgk_storage) {
    auto& file_manager = debug::file_manager::instance();
    auto& file16 = file_manager.get_file_stream(16);
    if(bgk_storage.u0 > 0 && bgk_storage.u_inflow > 0) {
        fmt::print("WARNING: inflow and volume force at the same time {} {} {}\n", bgk_storage.myrank, bgk_storage.u0, bgk_storage.u_inflow);
        file16 << "WARNING: inflow and volume force at the same time " << bgk_storage.myrank << " " << bgk_storage.u0 << " " << bgk_storage.u_inflow
               << "\n";
    }
    // Info section
    git_info();
    // Boundary conditions
#ifdef PERIODIC
    fmt::print("INFO: The test case has periodic bc (-DPERIODIC)\n");
    file16 << "INFO: The test case has periodic bc (-DPERIODIC)" << "\n";
#elif CHANNEL
    fmt::print("INFO: The test case has channel bc (-DCHANNEL)\n");
    file16 << "INFO: The test case has channel bc (-DCHANNEL)" << "\n";
#elif INFLOW
    fmt::print("INFO: The test case has inflow bc (-DINFLOW)\n");
    file16 << "INFO: The test case has inflow bc (-DINFLOW)" << "\n";
#elif MYVERSION
    fmt::print("INFO: The test case has ad-hoc bc (-DMYVERSION)\n");
    file16 << "INFO: The test case has ad-hoc bc (-DMYVERSION)" << "\n";
#else
    fmt::print("INFO: The test case is driven cavity (default)\n");
    file16 << "INFO: The test case is driven cavity (default)" << "\n";
#endif
#ifdef OBSTACLE
    fmt::print("INFO: The test case has an obstacle in the flow\n");
    file16 << "INFO: The test case has an obstacle in the flow" << "\n";
#endif
#ifdef NOSHIFT
    fmt::print("INFO: using NOSHIFT preprocessing flag\n");
    file16 << "INFO: using NOSHIFT preprocessing flag" << "\n";
#endif
#ifdef DRAG
    fmt::print("INFO: using DRAG preprocessing flag\n");
    file16 << "INFO: using DRAG preprocessing flag" << "\n";
    fmt::print("WARNING: the box for drag/lift is hard-coded, please check!\n");
    file16 << "WARNING: the box for drag/lift is hard-coded, please check!" << "\n";
#endif
#ifdef QUAD_P
    file16 << "INFO: using quad precision (storage)" << "\n";
#elif DOUBLE_P
    file16 << "INFO: using double precision (storage)" << "\n";
#elif HALF_P
    file16 << "INFO: using half precision (storage)" << "\n";
    fmt::print("WARNING: pure half precision has some problem\n");
#else
    file16 << "INFO: using single precision (storage)" << "\n";
#endif
#ifdef MIXEDPRECISION
    file16 << "INFO: using mixed precision" << "\n";
#else
    file16 << "INFO: using the same precision for computation" << "\n";
#endif
    file16 << "INFO: mykind= "    << utils::type_name<real_kinds::mykind>() << " range= " << std::numeric_limits<real_kinds::mykind>::max_exponent10 << "\n";
    file16 << "INFO: mykind= "    << utils::type_name<real_kinds::mykind>() << " huge= " << std::numeric_limits<real_kinds::mykind>::max() << "\n";
    file16 << "INFO: mykind= "    << utils::type_name<real_kinds::mykind>() << " epsilon= " << std::numeric_limits<real_kinds::mykind>::epsilon() << "\n";
    file16 << "INFO: mystorage= " << utils::type_name<real_kinds::mystorage>() << " range= " << std::numeric_limits<real_kinds::mystorage>::max_exponent10 << "\n";
    file16 << "INFO: mystorage= " << utils::type_name<real_kinds::mystorage>() << " huge= " << std::numeric_limits<real_kinds::mystorage>::max() << "\n";
    file16 << "INFO: mystorage= " << utils::type_name<real_kinds::mystorage>() << " epsilon= " << std::numeric_limits<real_kinds::mystorage>::epsilon() << "\n";
    file16 << "INFO: using RAW I/O" << "\n";
#ifdef NO_BINARY
    fmt::print("INFO: vtk output in ASCII (debug mode)\n");
    file16 << "INFO: vtk output in ASCII (debug mode)" << "\n";
#endif
#ifdef LES
    fmt::print("WARNING: LES (Smagorinsky) enabled UNDER DEVELOPMENT\n");
    file16 << "WARNING: LES (Smagorinsky) enabled UNDER DEVELOPMENT" << "\n";
#endif
#ifdef PGI
    file16 << "quad precision not supported" << "\n";
#endif
#ifdef DOCONCURRENT
    fmt::print("INFO: do concurrent version (GPU)\n");
    file16 << "INFO: do concurrent version (GPU)" << "\n";
#elif MULTICORE
    fmt::print("INFO: multicore parallelization (CPU)\n");
    file16 << "INFO: multicore parallelization (CPU)" << "\n";
#elif OFFLOAD
    fmt::print("INFO: offload version (GPU)\n");
    file16 << "INFO: offload version (GPU)" << "\n";
#elif OPENACC
    fmt::print("INFO: openacc version (GPU)\n");
    file16 << "INFO: openacc version (GPU)" << "\n";
#else
    fmt::print("INFO: serial version (CPU)\n");
    file16 << "INFO: serial version (CPU)" << "\n";
#endif
#ifdef FUSED
    file16 << "INFO: using Fused version" << "\n";
#else
    file16 << "INFO: using Move & collide (Original) version" << "\n";
#endif
#ifdef DEBUG_3
    if(bgk_storage.myrank == 0) { fmt::print("INFO: DEBUG3 mode enabled\n"); }
#endif
#ifdef DEBUG_2
    if(bgk_storage.myrank == 0) { fmt::print("INFO: DEBUG2 mode enabled\n"); }
#endif
#ifdef DEBUG_1
    if(bgk_storage.myrank == 0) {
        fmt::print("INFO: DEBUG1 mode enabled\n");
        fmt::print("DEBUG1: Exiting from sub. check_case\n");
    }
#endif
#ifdef TRICK1
    fmt::print("WARNING: square box mandatory (TRICK1)!\n");
    file16 << "WARNING: square box mandatory (TRICK1)!" << "\n";
    int l = 1, m = 1; // Example values
    if(l != m) {
        fmt::print("ERROR: box not squared (TRICK1)!\n");
        file16 << "ERROR: box not squared (TRICK1)!" << "\n";
        return 1;
    }
#endif
#ifdef TRICK2
    fmt::print("WARNING: forced offload num_threads(TRICK2)!\n");
    file16 << "WARNING: forced offload num_threads(TRICK2)!" << "\n";
#endif

#ifdef DEBUG
    file16.flush();
#endif
}

}
