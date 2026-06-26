#include <algorithm>
#include <cmath>
#include <cstddef>
#include <stdexcept>
#include <string>
#include <vector>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <sycl/sycl.hpp>

#include "dpctl4pybind11.hpp"

namespace py = pybind11;

std::string get_device_name(const sycl::device &dev) {
    return dev.get_info<sycl::info::device::name>();
}

std::vector<float> vector_add_kernel(const sycl::device &dev, std::size_t n) {
    sycl::queue q(dev);

    float *a = sycl::malloc_shared<float>(n, q);
    float *b = sycl::malloc_shared<float>(n, q);
    float *c = sycl::malloc_shared<float>(n, q);

    if (!a || !b || !c) {
        throw std::runtime_error("USM allocation failed");
    }

    for (std::size_t i = 0; i < n; ++i) {
        a[i] = static_cast<float>(i);
        b[i] = 1.0f;
        c[i] = 0.0f;
    }

    q.parallel_for(sycl::range<1>(n), [=](sycl::id<1> idx) {
        std::size_t i = idx[0];
        c[i] = a[i] + b[i];
    }).wait();

    std::vector<float> result(n);
    for (std::size_t i = 0; i < n; ++i) {
        result[i] = c[i];
    }

    sycl::free(a, q);
    sycl::free(b, q);
    sycl::free(c, q);

    return result;
}

py::dict lbm_density_kernel(const sycl::device &dev, int nx, int ny) {
    if (nx <= 0 || ny <= 0) {
        throw std::runtime_error("nx and ny must be positive");
    }

    sycl::queue q(dev);
    std::size_t n = static_cast<std::size_t>(nx) * static_cast<std::size_t>(ny);

    float *f0 = sycl::malloc_shared<float>(n, q);
    float *f1 = sycl::malloc_shared<float>(n, q);
    float *f2 = sycl::malloc_shared<float>(n, q);
    float *f3 = sycl::malloc_shared<float>(n, q);
    float *f4 = sycl::malloc_shared<float>(n, q);
    float *f5 = sycl::malloc_shared<float>(n, q);
    float *f6 = sycl::malloc_shared<float>(n, q);
    float *f7 = sycl::malloc_shared<float>(n, q);
    float *f8 = sycl::malloc_shared<float>(n, q);

    float *rho = sycl::malloc_shared<float>(n, q);
    float *ux = sycl::malloc_shared<float>(n, q);
    float *uy = sycl::malloc_shared<float>(n, q);

    if (!f0 || !f1 || !f2 || !f3 || !f4 || !f5 || !f6 || !f7 || !f8 ||
        !rho || !ux || !uy) {
        throw std::runtime_error("USM allocation failed");
    }

    for (std::size_t i = 0; i < n; ++i) {
        f0[i] = 4.0f / 9.0f;

        f1[i] = 1.0f / 9.0f;
        f2[i] = 1.0f / 9.0f;
        f3[i] = 1.0f / 9.0f;
        f4[i] = 1.0f / 9.0f;

        f5[i] = 1.0f / 36.0f;
        f6[i] = 1.0f / 36.0f;
        f7[i] = 1.0f / 36.0f;
        f8[i] = 1.0f / 36.0f;

        rho[i] = 0.0f;
        ux[i] = 0.0f;
        uy[i] = 0.0f;
    }

    q.parallel_for(sycl::range<1>(n), [=](sycl::id<1> idx) {
        std::size_t i = idx[0];

        float local_rho =
            f0[i] + f1[i] + f2[i] + f3[i] + f4[i] +
            f5[i] + f6[i] + f7[i] + f8[i];

        rho[i] = local_rho;

        ux[i] = (f1[i] + f5[i] + f8[i] - f3[i] - f6[i] - f7[i]) / local_rho;
        uy[i] = (f2[i] + f5[i] + f6[i] - f4[i] - f7[i] - f8[i]) / local_rho;
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

    sycl::free(f0, q);
    sycl::free(f1, q);
    sycl::free(f2, q);
    sycl::free(f3, q);
    sycl::free(f4, q);
    sycl::free(f5, q);
    sycl::free(f6, q);
    sycl::free(f7, q);
    sycl::free(f8, q);

    sycl::free(rho, q);
    sycl::free(ux, q);
    sycl::free(uy, q);

    py::dict result;
    result["nx"] = nx;
    result["ny"] = ny;
    result["rho_min"] = rho_min;
    result["rho_max"] = rho_max;
    result["max_abs_ux"] = max_abs_ux;
    result["max_abs_uy"] = max_abs_uy;

    return result;
}

PYBIND11_MODULE(_minilb_dpctl, m) {
    m.doc() = "miniLB dpctl + pybind11 + C++ SYCL kernel examples";

    m.def("get_device_name", &get_device_name,
          "Return the name of a dpctl/SYCL device");

    m.def("vector_add_kernel", &vector_add_kernel,
          "Run a C++ SYCL vector addition kernel using a dpctl device");

    m.def("lbm_density_kernel", &lbm_density_kernel,
          "Run a miniLB-style D2Q9 density/velocity SYCL kernel using a dpctl device");
}
