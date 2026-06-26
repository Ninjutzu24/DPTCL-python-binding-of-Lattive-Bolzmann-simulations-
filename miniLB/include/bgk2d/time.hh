#ifndef __TIME_H__
#define __TIME_H__

#include <precision.hh>
#include <string>

#ifdef PGI
// Do nothing
#else
// Include check_mem equivalent in C++, if exists
#include <get_mem.hh>
#endif

namespace bgk::utils {

struct system_clock_t {
    long int countH0;
    long int count_rate;
    long int count_max;
};

void system_clock(system_clock_t &clock);

void system_clock(long int &countH0, long int &count_rate, long int &count_max);

// Global variables ported as singleton struct
struct timing {
  private:
    timing() = default;
    timing(const timing&) = delete;
    timing& operator=(const timing&) = delete;

  public:
    // Assuming real_kinds and check_mem functionalities are handled or not needed
    // in C++ version

    real_kinds::sp time_init, time_init1;
    real_kinds::sp time_coll, time_coll1;
    real_kinds::sp time_loop, time_loop1;
    real_kinds::sp time_move, time_move1;
    real_kinds::sp time_obs, time_obs1;
    real_kinds::sp time_bc, time_bc1;
    real_kinds::sp time_mp, time_mp1;
    real_kinds::sp time_dg, time_dg1;
    real_kinds::sp time_dev, time_dev1;
    real_kinds::sp time_io, time_io1;
    real_kinds::sp time_inn_loop, time_inn_loop1;
    real_kinds::sp timeZ, timeY, timeX;

    long int count_rate, count_max;
    long int count1, count0;
    long int count2, count3;
    long int countA0, countB0, countC0, countD0, countE0, countF0, countO0;
    long int countA1, countB1, countC1, countD1, countE1, countF1, countO1;
    long int countH0;
    long int countH1;

    real_kinds::mykind tcount1, tcount0;
    real_kinds::mykind tcount2, tcount3;

    real_kinds::sp tcountA0, tcountB0, tcountC0;
    real_kinds::sp tcountD0, tcountE0, tcountF0;
    real_kinds::sp tcountG0, tcountH0;
    real_kinds::sp tcountO0;
    real_kinds::sp tcountA1, tcountB1, tcountC1;
    real_kinds::sp tcountD1, tcountE1, tcountF1;
    real_kinds::sp tcountG1, tcountH1;
    real_kinds::sp tcountO1;
    real_kinds::sp old1, old2, old3, old4, old5, old6;
    real_kinds::sp tcountZ0, tcountZ1;
    real_kinds::sp tcountY0, tcountY1;
    real_kinds::sp tcountX0, tcountX1;

    real_kinds::dp mem_start, mem_stop; // double precision

    static auto &instance() {
        static timing tm{};
        return tm;
    };
};

void time(real_kinds::sp &t);

// Get current date using UNIX date command format
std::string get_date();

} // namespace bgk::utils
#endif // __TIME_H__