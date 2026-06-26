#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <sycl/sycl.hpp>

#include "dpctl4pybind11.hpp"

namespace py = pybind11;

std::string get_queue_device_name(sycl::queue q) {
    return q.get_device().get_info<sycl::info::device::name>();
}

float *get_usm_float_pointer(const py::object &usm_obj, const std::string &name) {
    if (!py::hasattr(usm_obj, "__sycl_usm_array_interface__")) {
        throw std::runtime_error(name + " does not expose __sycl_usm_array_interface__");
    }

    py::dict iface = usm_obj.attr("__sycl_usm_array_interface__");
    py::tuple data = iface["data"].cast<py::tuple>();

    std::uintptr_t ptr_value = data[0].cast<std::uintptr_t>();

    if (ptr_value == 0) {
        throw std::runtime_error(name + " has a null USM pointer");
    }

    return reinterpret_cast<float *>(ptr_value);
}

py::dict run_minilb_step(
    sycl::queue q,
    py::object f_obj,
    py::object f_new_obj,
    py::object f_stream_obj,
    py::object rho_obj,
    py::object ux_obj,
    py::object uy_obj,
    int nx,
    int ny,
    float omega,
    float u_lid
) {
    if (nx <= 2 || ny <= 2) {
        throw std::runtime_error("nx and ny must be greater than 2");
    }

    const std::size_t n =
        static_cast<std::size_t>(nx) * static_cast<std::size_t>(ny);

    const std::size_t center =
        static_cast<std::size_t>(ny / 2) * static_cast<std::size_t>(nx)
        + static_cast<std::size_t>(nx / 2);

    /*
     Important:
     Python allocates all main arrays using dpctl.memory.MemoryUSMShared.
     C++ only receives the USM memory objects and launches SYCL kernels.
    */
    float *f = get_usm_float_pointer(f_obj, "f");
    float *f_new = get_usm_float_pointer(f_new_obj, "f_new");
    float *f_stream = get_usm_float_pointer(f_stream_obj, "f_stream");
    float *rho = get_usm_float_pointer(rho_obj, "rho");
    float *ux = get_usm_float_pointer(ux_obj, "ux");
    float *uy = get_usm_float_pointer(uy_obj, "uy");

    // Kernel 1: compute rho, ux, uy.
    q.parallel_for(sycl::range<1>(n), [=](sycl::id<1> idx) {
        std::size_t i = idx[0];

        float f0 = f[0 * n + i];
        float f1 = f[1 * n + i];
        float f2 = f[2 * n + i];
        float f3 = f[3 * n + i];
        float f4 = f[4 * n + i];
        float f5 = f[5 * n + i];
        float f6 = f[6 * n + i];
        float f7 = f[7 * n + i];
        float f8 = f[8 * n + i];

        float local_rho = f0 + f1 + f2 + f3 + f4 + f5 + f6 + f7 + f8;

        rho[i] = local_rho;
        ux[i] = (f1 + f5 + f8 - f3 - f6 - f7) / local_rho;
        uy[i] = (f2 + f5 + f6 - f4 - f7 - f8) / local_rho;
    }).wait();

    // Kernel 2: BGK collision for all D2Q9 populations.
    q.parallel_for(sycl::range<1>(9 * n), [=](sycl::id<1> idx) {
        std::size_t gid = idx[0];

        int k = static_cast<int>(gid / n);
        std::size_t i = gid % n;

        float cx = 0.0f;
        float cy = 0.0f;
        float w = 0.0f;

        if (k == 0) {
            cx = 0.0f;  cy = 0.0f;  w = 4.0f / 9.0f;
        } else if (k == 1) {
            cx = 1.0f;  cy = 0.0f;  w = 1.0f / 9.0f;
        } else if (k == 2) {
            cx = 0.0f;  cy = 1.0f;  w = 1.0f / 9.0f;
        } else if (k == 3) {
            cx = -1.0f; cy = 0.0f;  w = 1.0f / 9.0f;
        } else if (k == 4) {
            cx = 0.0f;  cy = -1.0f; w = 1.0f / 9.0f;
        } else if (k == 5) {
            cx = 1.0f;  cy = 1.0f;  w = 1.0f / 36.0f;
        } else if (k == 6) {
            cx = -1.0f; cy = 1.0f;  w = 1.0f / 36.0f;
        } else if (k == 7) {
            cx = -1.0f; cy = -1.0f; w = 1.0f / 36.0f;
        } else {
            cx = 1.0f;  cy = -1.0f; w = 1.0f / 36.0f;
        }

        float local_rho = rho[i];
        float local_ux = ux[i];
        float local_uy = uy[i];

        float cu = cx * local_ux + cy * local_uy;
        float u2 = local_ux * local_ux + local_uy * local_uy;

        float feq =
            w * local_rho *
            (1.0f + 3.0f * cu + 4.5f * cu * cu - 1.5f * u2);

        f_new[k * n + i] =
            f[k * n + i] - omega * (f[k * n + i] - feq);
    }).wait();

    // Kernel 3: streaming with bounce-back walls.
    // If a population would leave the domain, it is reflected into the opposite direction.
    q.parallel_for(sycl::range<1>(9 * n), [=](sycl::id<1> idx) {
        std::size_t gid = idx[0];

        int k = static_cast<int>(gid / n);
        std::size_t i = gid % n;

        int x = static_cast<int>(i % static_cast<std::size_t>(nx));
        int y = static_cast<int>(i / static_cast<std::size_t>(nx));

        int dx = 0;
        int dy = 0;

        if (k == 0) {
            dx = 0;  dy = 0;
        } else if (k == 1) {
            dx = 1;  dy = 0;
        } else if (k == 2) {
            dx = 0;  dy = 1;
        } else if (k == 3) {
            dx = -1; dy = 0;
        } else if (k == 4) {
            dx = 0;  dy = -1;
        } else if (k == 5) {
            dx = 1;  dy = 1;
        } else if (k == 6) {
            dx = -1; dy = 1;
        } else if (k == 7) {
            dx = -1; dy = -1;
        } else {
            dx = 1;  dy = -1;
        }

        int x_to = x + dx;
        int y_to = y + dy;

        bool inside = (x_to >= 0 && x_to < nx && y_to >= 0 && y_to < ny);

        if (inside) {
            std::size_t dst =
                static_cast<std::size_t>(y_to) * static_cast<std::size_t>(nx)
                + static_cast<std::size_t>(x_to);

            f_stream[k * n + dst] = f_new[k * n + i];
        } else {
            int k_opp = 0;

            if (k == 1) k_opp = 3;
            else if (k == 2) k_opp = 4;
            else if (k == 3) k_opp = 1;
            else if (k == 4) k_opp = 2;
            else if (k == 5) k_opp = 7;
            else if (k == 6) k_opp = 8;
            else if (k == 7) k_opp = 5;
            else if (k == 8) k_opp = 6;

            f_stream[k_opp * n + i] = f_new[k * n + i];
        }
    }).wait();

    // Kernel 4: moving lid boundary at top wall.
    q.parallel_for(sycl::range<1>(static_cast<std::size_t>(nx)), [=](sycl::id<1> idx) {
        int x = static_cast<int>(idx[0]);
        int y = ny - 1;

        std::size_t i =
            static_cast<std::size_t>(y) * static_cast<std::size_t>(nx)
            + static_cast<std::size_t>(x);

        float local_rho = rho[i];

        // Unknown downward populations at the top wall: f4, f7, f8.
        f_stream[4 * n + i] = f_new[2 * n + i];
        f_stream[7 * n + i] = f_new[5 * n + i] - (1.0f / 6.0f) * local_rho * u_lid;
        f_stream[8 * n + i] = f_new[6 * n + i] + (1.0f / 6.0f) * local_rho * u_lid;
    }).wait();

    float rho_min = rho[0];
    float rho_max = rho[0];
    float max_abs_ux = std::fabs(ux[0]);
    float max_abs_uy = std::fabs(uy[0]);

    for (std::size_t i = 1; i < n; ++i) {
        rho_min = std::min(rho_min, rho[i]);
        rho_max = std::max(rho_max, rho[i]);
        max_abs_ux = std::max(max_abs_ux, std::fabs(ux[i]));
        max_abs_uy = std::max(max_abs_uy, std::fabs(uy[i]));
    }

    // Check collision for center f1.
    float center_rho = rho[center];
    float center_ux = ux[center];
    float center_uy = uy[center];

    float cx1 = 1.0f;
    float cy1 = 0.0f;
    float w1 = 1.0f / 9.0f;

    float cu1 = cx1 * center_ux + cy1 * center_uy;
    float u2_center = center_ux * center_ux + center_uy * center_uy;

    float feq1_center =
        w1 * center_rho *
        (1.0f + 3.0f * cu1 + 4.5f * cu1 * cu1 - 1.5f * u2_center);

    float expected_f1_center_after_collision =
        f[1 * n + center] - omega * (f[1 * n + center] - feq1_center);

    float actual_f1_center_after_collision = f_new[1 * n + center];

    bool collision_check_ok =
        std::fabs(actual_f1_center_after_collision - expected_f1_center_after_collision) < 1e-5f;

    // Check bounce-back at left wall.
    int left_x = 0;
    int left_y = ny / 2;

    std::size_t left_cell =
        static_cast<std::size_t>(left_y) * static_cast<std::size_t>(nx)
        + static_cast<std::size_t>(left_x);

    float expected_left_bounce = f_new[3 * n + left_cell];
    float actual_left_bounce = f_stream[1 * n + left_cell];

    bool bounce_back_check_ok =
        std::fabs(actual_left_bounce - expected_left_bounce) < 1e-5f;

    // Check moving lid at top center.
    int top_x = nx / 2;
    int top_y = ny - 1;

    std::size_t top_cell =
        static_cast<std::size_t>(top_y) * static_cast<std::size_t>(nx)
        + static_cast<std::size_t>(top_x);

    float expected_top_f8 =
        f_new[6 * n + top_cell] + (1.0f / 6.0f) * rho[top_cell] * u_lid;

    float actual_top_f8 = f_stream[8 * n + top_cell];

    bool moving_lid_check_ok =
        std::fabs(actual_top_f8 - expected_top_f8) < 1e-5f;

    bool check_ok =
        collision_check_ok &&
        bounce_back_check_ok &&
        moving_lid_check_ok;

    py::dict result;

    result["nx"] = nx;
    result["ny"] = ny;

    result["rho_min"] = rho_min;
    result["rho_max"] = rho_max;
    result["max_abs_ux"] = max_abs_ux;
    result["max_abs_uy"] = max_abs_uy;

    result["center_rho"] = center_rho;
    result["center_ux"] = center_ux;
    result["center_uy"] = center_uy;

    result["center_f1_before"] = f[1 * n + center];
    result["center_f1_after_collision"] = actual_f1_center_after_collision;
    result["center_f1_expected_collision"] = expected_f1_center_after_collision;

    result["left_wall_expected_bounce"] = expected_left_bounce;
    result["left_wall_actual_bounce"] = actual_left_bounce;

    result["top_lid_expected_f8"] = expected_top_f8;
    result["top_lid_actual_f8"] = actual_top_f8;

    result["collision_all_populations"] = true;
    result["streaming_all_populations"] = true;
    result["periodic_boundaries"] = false;
    result["bounce_back_walls"] = true;
    result["moving_lid"] = true;
    result["u_lid"] = u_lid;

    result["memory_allocated_in_python_with_dpctl"] = true;
    result["kernels_launched_in_cpp_with_pybind11"] = true;

    result["collision_check_ok"] = collision_check_ok;
    result["bounce_back_check_ok"] = bounce_back_check_ok;
    result["moving_lid_check_ok"] = moving_lid_check_ok;
    result["check_ok"] = check_ok;

    return result;
}



py::dict run_minilb_steps(
    sycl::queue q,
    py::object f_obj,
    py::object f_new_obj,
    py::object f_stream_obj,
    py::object rho_obj,
    py::object ux_obj,
    py::object uy_obj,
    int nx,
    int ny,
    float omega,
    float u_lid,
    int steps
) {
    if (steps <= 0) {
        throw std::runtime_error("steps must be positive");
    }

    if (nx <= 2 || ny <= 2) {
        throw std::runtime_error("nx and ny must be greater than 2");
    }

    const std::size_t n =
        static_cast<std::size_t>(nx) * static_cast<std::size_t>(ny);

    const std::size_t f_bytes = 9 * n * sizeof(float);

    float *f = get_usm_float_pointer(f_obj, "f");
    float *f_stream = get_usm_float_pointer(f_stream_obj, "f_stream");

    py::dict last_result;

    q.wait();
    auto t0 = std::chrono::high_resolution_clock::now();

    for (int step = 0; step < steps; ++step) {
        last_result = run_minilb_step(
            q,
            f_obj,
            f_new_obj,
            f_stream_obj,
            rho_obj,
            ux_obj,
            uy_obj,
            nx,
            ny,
            omega,
            u_lid
        );

        /*
         After one V5 step:
           f_stream contains the streamed distributions.
         For the next iteration, f_stream becomes the new input f.
         This copy is performed inside C++ using the SYCL queue.
        */
        if (step + 1 < steps) {
            q.memcpy(f, f_stream, f_bytes).wait();
        }
    }

    q.wait();
    auto t1 = std::chrono::high_resolution_clock::now();

    double elapsed_seconds =
        std::chrono::duration<double>(t1 - t0).count();

    double mlups =
        (static_cast<double>(nx) *
         static_cast<double>(ny) *
         static_cast<double>(steps)) /
        elapsed_seconds /
        1000000.0;

    py::dict result;

    result["nx"] = nx;
    result["ny"] = ny;
    result["steps"] = steps;
    result["elapsed_seconds"] = elapsed_seconds;
    result["mlups"] = mlups;

    result["memory_allocated_in_python_with_dpctl"] = true;
    result["kernels_launched_in_cpp_with_pybind11"] = true;
    result["loop_executed_in_cpp"] = true;

    result["last_step_check_ok"] = last_result["check_ok"];
    result["last_step_collision_check_ok"] = last_result["collision_check_ok"];
    result["last_step_bounce_back_check_ok"] = last_result["bounce_back_check_ok"];
    result["last_step_moving_lid_check_ok"] = last_result["moving_lid_check_ok"];

    return result;
}

PYBIND11_MODULE(_minilb_step, m) {
    m.doc() = "miniLB V5 pybind11 module: Python allocates USM with dpctl, C++ launches SYCL kernels";

    m.def("get_queue_device_name", &get_queue_device_name,
          "Return the name of the device associated with a dpctl/SYCL queue");

    m.def("run_minilb_step", &run_minilb_step,
          "Run a simplified Lid-Driven Cavity LBM step with density, collision, streaming and boundary conditions using Python-allocated USM memory");

    m.def("run_minilb_steps", &run_minilb_steps,
          "Run multiple miniLB V5 steps in C++ for benchmarking");

}
