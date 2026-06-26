#include <iomanip>
#include <iostream>
#include <time.hh>
#include <chrono>

void bgk::utils::time(real_kinds::sp &t) {
  auto now = std::chrono::system_clock::now();
  auto now_c = std::chrono::system_clock::to_time_t(now);
  std::tm now_tm = *std::localtime(&now_c);

  t = 3600.f * now_tm.tm_hour + 60.f * now_tm.tm_min + now_tm.tm_sec +
      std::chrono::duration_cast<std::chrono::milliseconds>(
          now.time_since_epoch())
              .count() %
          1000 / 1000.f;
}

void bgk::utils::system_clock(system_clock_t &clock){
    auto now = std::chrono::system_clock::now();
    clock.countH0 = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();
    // Assuming clock rate as nanoseconds per second (1e9)
    clock.count_rate = 1000000000LL; // 1e9
    // Maximum Clock Value
    clock.count_max = std::chrono::nanoseconds::max().count();
}

void bgk::utils::system_clock(long int& countH0, long int& count_rate, long int& count_max){
    auto now = std::chrono::system_clock::now();
    countH0 = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();
    // Assuming clock rate as nanoseconds per second (1e9)
    count_rate = 1000000000LL; // 1e9
    // Maximum Clock Value
    count_max = std::chrono::nanoseconds::max().count();
}

std::string bgk::utils::get_date(){
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);

    std::ostringstream ss;
    // Use std::put_time to format the output similar to the `date` command
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X") << "\n";
    return ss.str();
}