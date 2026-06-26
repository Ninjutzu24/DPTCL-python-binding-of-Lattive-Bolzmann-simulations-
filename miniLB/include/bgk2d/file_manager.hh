#ifndef __FILE_MANAGER_H__
#define __FILE_MANAGER_H__

#include <fmt/format.h> //For formatting strings
#include <fstream>
#include <iomanip>
#include <iostream>
#include <unordered_map>
// To speed up the porting process
#include <frozen/string.h>
#include <frozen/unordered_map.h>

namespace bgk {

namespace utils {
template<int N>
struct string_literal_wrapper {
    constexpr string_literal_wrapper(const char (&s)[N]) { std::copy_n(s, N, value); }

    char value[N];
};
} // namespace utils

namespace files {
static constexpr std::string_view input_file = "bgk.input";
}

namespace debug {

namespace files {


static constexpr std::array<frozen::string, 11> filenames = {"time.log", "bgk.log", "prof_i.dat", "probe.dat",
    "task.log", "prof_j.dat", "probe_visc.dat", "drag.lift.dat", "bgk.time.log", "diagno.dat", "u_med.dat"};

static constexpr uint8_t time_log = 0;
static constexpr uint8_t bgk_log = 16;
static constexpr uint8_t prof_i_dat = 61;
static constexpr uint8_t probe_dat = 68;
static constexpr uint8_t task_log = 38;
static constexpr uint8_t prof_j_dat = 64;
static constexpr uint8_t probe_visc_dat = 65;
static constexpr uint8_t drag_lift_dat = 66;
static constexpr uint8_t bgk_time_log = 99;
static constexpr uint8_t diagno_dat = 63;
static constexpr uint8_t u_med_dat = 62;

// Utility to easy translation from fortran code
static constexpr frozen::unordered_map<uint8_t, uint8_t, 11> id_to_filename
    = {{0, 0}, {16, 1}, {61, 2}, {68, 3}, {38, 4}, {64, 5}, {65, 6}, {66, 7}, {99, 8}, {63, 9}, {62, 10}};

} // namespace files

/**
 * @brief Simple file manager with fixed files and paths
    Used for debug information
 *
 */
struct file_manager {
  private:
    file_manager() {
#pragma unroll
        for(int i = 0; i < files.size(); i++) {
            files[i] = std::ofstream(files::filenames[i].data(), std::ios::out | std::ios::trunc);
            files[i] << std::setprecision(7);
        }
    }

    ~file_manager() {
#pragma unroll
        for(int i = 0; i < files.size(); i++) { files[i].close(); }
    }

  public:
    template<utils::string_literal_wrapper format_string, typename... Args>
    void write_format(uint8_t id, Args ...args) {
        using namespace files;
        constexpr auto string = format_string.value;
        files[id_to_filename.at(id)] << fmt::vformat(string, fmt::make_format_args(args...));
        #ifdef DEBUG
            files[id_to_filename.at(id)].flush();
        #endif
    }

    template<typename... Args>
    void write(uint8_t id, std::string_view data) {
        using namespace files;
        files[id_to_filename.at(id)] << data << "\n";
        #ifdef DEBUG
            files[id_to_filename.at(id)].flush();
        #endif
    }

    template<typename... Args>
    void write(uint8_t id, Args &&...args) {
        using namespace files;
        auto &stream = files[id_to_filename.at(id)];
        (stream << ... << args);
        #ifdef DEBUG
            files[id_to_filename.at(id)].flush();
        #endif
    }


    void flush(uint8_t id) { files[files::id_to_filename.at(id)].flush(); }

    static auto &instance() {
        static file_manager fm{};
        return fm;
    }

    auto &get_file_stream(uint8_t id) { return files[files::id_to_filename.at(id)]; }

  private:
    std::array<std::ofstream, 11> files;
};


} // namespace debug
} // namespace bgk

#endif // __FILE_MANAGER_H__