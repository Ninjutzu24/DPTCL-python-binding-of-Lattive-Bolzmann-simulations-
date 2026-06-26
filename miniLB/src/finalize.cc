#include <finalize.hh>
#include <fstream>
#include <fmt/core.h>
#include <time.hh>

namespace bgk {
void finalize(storage &bgk_storage, const int itstart, const int itfin) {
    if(bgk_storage.myrank == 0) {
        std::ofstream file("bgk.perf");
        auto& timing = utils::timing::instance();

        file << "#--------------------------------\n";
        file << "# Run info \n";
        file << bgk_storage.l << " " << bgk_storage.m << "\n";
        file << "#--------------------------------\n";
        file << "# Time for section \n";
        file << fmt::format(" # init   time {:14.6e} {:14.6e}\n", timing.time_init, timing.time_init1);
        file << fmt::format(" # loop   time {:14.6e} {:14.6e}\n", timing.time_loop, timing.time_loop1);
        file << fmt::format(" # coll   time {:14.6e} {:14.6e}\n", timing.time_coll, timing.time_coll1);
        file << fmt::format(" # move   time {:14.6e} {:14.6e}\n", timing.time_move, timing.time_move1);
        file << fmt::format(" # bc     time {:14.6e} {:14.6e}\n", timing.time_bc, timing.time_bc1);
        file << fmt::format(" # I/O    time {:14.6e} {:14.6e}\n", timing.time_io, timing.time_io1);
        file << fmt::format(" # diagno time {:14.6e} {:14.6e}\n", timing.time_dg, timing.time_dg1);
        file << fmt::format(" # Obst   time {:14.6e} {:14.6e}\n", timing.time_obs, timing.time_obs1);
        file << fmt::format(" # Check      {:14.6e}\n",
            timing.time_loop - (timing.time_coll + timing.time_move + timing.time_bc + timing.time_io + timing.time_dg + timing.time_mp + timing.time_obs));
        file << "#--------------------------------\n";
        file << "# Ratio per section (loop)\n";
        file << fmt::format(" # Ratio I/O  {:7.3f} {:7.3f}\n", timing.time_io / timing.time_loop, timing.time_io1 / timing.time_loop1);
        file << fmt::format(" # Ratio BC   {:7.3f} {:7.3f}\n", timing.time_bc / timing.time_loop, timing.time_bc1 / timing.time_loop1);
        file << fmt::format(" # Ratio Coll {:7.3f} {:7.3f}\n", timing.time_coll / timing.time_loop, timing.time_coll1 / timing.time_loop1);
        file << fmt::format(" # Ratio Diag.{:7.3f} {:7.3f}\n", timing.time_dg / timing.time_loop, timing.time_dg1 / timing.time_loop1);
        file << fmt::format(" # Ratio Move {:7.3f} {:7.3f}\n", timing.time_move / timing.time_loop, timing.time_move1 / timing.time_loop1);
        file << fmt::format(" # Ratio Obs  {:7.3f} {:7.3f}\n", timing.time_obs / timing.time_loop, timing.time_obs1 / timing.time_loop1);
        file << "#--------------------------------\n";
        file << "# Derived (global) metrics \n";
        file << fmt::format(
            " # Mlups      {:14.6f}\n", static_cast<float>(bgk_storage.l) * static_cast<float>(bgk_storage.m) * (itfin - itstart) / (timing.time_loop1) / 1000.0 / 1000.0);
        file << fmt::format(" # coll   Flop{:14.6e} MFlops\n",
            static_cast<float>(130) * static_cast<float>(bgk_storage.l) * static_cast<float>(bgk_storage.m) * (itfin - itstart) / (timing.time_coll) / 1000.0 / 1000.0);
        file << fmt::format(" # coll   BW  {:14.6e} GB/s\n",
            static_cast<float>(9 * 8) * static_cast<float>(bgk_storage.l) * static_cast<float>(bgk_storage.m) * (itfin - itstart) / (timing.time_coll) / 1000.0 / 1000.0);
        file << fmt::format(" # move   BW  {:14.6e} GB/s\n",
            static_cast<float>(8 * 8) * static_cast<float>(bgk_storage.l) * static_cast<float>(bgk_storage.m) * (itfin - itstart) / (timing.time_move) / 1000.0 / 1000.0);
        file << "#--------------------------------\n";
        file << "# Memory (task 0) metrics \n";
        file << fmt::format(" # Memory (start,stop){:14.6f} {:14.6f} MB\n", timing.mem_start, timing.mem_stop);
        file << fmt::format("{:14.6f}\n", (bgk_storage.l / 1024.0) * (bgk_storage.m / 1024.0) * 9 * 2 * 4);
        file << "#--------------------------------\n";
        file.close();

        fmt::print(
            " # Mlups      {:14.6f}\n", static_cast<float>(bgk_storage.l) * static_cast<float>(bgk_storage.m) * (itfin - itstart) / (timing.time_loop1) / 1000.0 / 1000.0);
    }

    // Closing files

#ifdef DEBUG_1
    if(bgk_storage.myrank == 0) { fmt::print("DEBUG1: Exiting from sub. finalize\n"); }
#endif

#ifdef MEM_CHECK
    if(bgk_storage.myrank == 0) {
        timing.mem_stop = get_mem();
        fmt::print("MEM_CHECK: after sub. finalize mem = {}\n", timing.mem_stop);
    }
#endif
}
} // namespace bgk