#ifndef __STORAGE_H__
#define __STORAGE_H__


#include <array>
#include <memory>
#include <precision.hh>

#include <mdspan/mdspan.hpp>
#include <sycl/sycl.hpp>
#include <sycl/usm_buffer.hh>

namespace bgk {

    #if defined(BGK_SYCL_LAYOUT_RIGHT)
        using layout = Kokkos::layout_right;
    #elif defined(BGK_SYCL_LAYOUT_LEFT)
        using layout = Kokkos::layout_left;
    #else
        using layout = Kokkos::layout_left;
    #endif

struct storage {
    // MPI and other constants
    static constexpr int mpid = 2;
    static constexpr int zeroi = 0;
    static constexpr int unoi = 1;
    static constexpr int border = 32;

    // Global and local sizes
    int lx, ly;
    int l, m;
    int l1, m1;
    std::array<int, 2> left, right, front, rear;

// Precision-dependent constants
#if defined(PGI) || defined(ARM)
    using qp = dp; // Fallback to double precision for PGI or ARM
#else
    using qp = real_kinds::qp;
#endif

    static constexpr qp zero_qp = 0.0;
    static constexpr qp uno_qp = 1.0;
    static constexpr qp tre_qp = 3.0;

    static constexpr qp rf_qp = 3.0;
    static constexpr qp qf_qp = 1.5;

    static constexpr qp p0_qp = 4.0 / 9.0;
    static constexpr qp p1_qp = 1.0 / 9.0;
    static constexpr qp p2_qp = uno_qp / (rf_qp * rf_qp * (uno_qp + tre_qp));

    // Obstacle and MPI related
    int myrank;
    int imax, imin, jmax, jmin;
    int nobs;
    std::array<int, 2> offset;
    int ipad, jpad;
    int flag1, flag2, flag3;

    // Clang-format off
    //  Direction vectors and their integer counterparts
    static constexpr std::array<real_kinds::mykind, 19> cx{
        1, 1, 1, 1, 1, 0, 0, 0, 0, -1, -1, -1, -1, -1, 0, 0, 0, 0, 0};
    static constexpr std::array<real_kinds::mykind, 19> cy{
        -1, 0, 1, 0, 0, 0, 1, 1, 1, -1, 0, 1, 0, 0, 0, -1, -1, -1, 0};
    static constexpr std::array<int, 19> icx{1, 1, 1, 1, 1, 0, 0, 0, 0, -1, -1, -1, -1, -1, 0, 0, 0, 0, 0};
    static constexpr std::array<int, 19> icy{-1, 0, 1, 0, 0, 0, 1, 1, 1, -1, 0, 1, 0, 0, 0, -1, -1, -1, 0};

    // Clang-format on
    // Offloading variables
    std::shared_ptr<sycl::queue> host_q;
    std::shared_ptr<sycl::queue> dev_q;
#ifdef FUSED
    // Pointers for fused storage
    // std::unique_ptr<std::vector<std::vector<mystorage>>> a01, a03, a05, a08, a10, a12, a14, a17, a19;
    // std::unique_ptr<std::vector<std::vector<mystorage>>> b01, b03, b05, b08, b10, b12, b14, b17, b19;
    // std::unique_ptr<std::vector<std::vector<mystorage>>> c01, c03, c05, c08, c10, c12, c14, c17, c19;
    #if defined(BGK_SYCL_MALLOC_SHARED)
    #ifdef BGK_SYCL_ENABLE_PREFETCH
    bgk::usm_buffer<real_kinds::mystorage, sycl::usm::alloc::shared, true> _a01, _a03, _a05, _a08, _a10, _a12, _a14, _a17,
        _a19;
    bgk::usm_buffer<real_kinds::mystorage, sycl::usm::alloc::shared, true> _b01, _b03, _b05, _b08, _b10, _b12, _b14, _b17,
        _b19;
    #else
    bgk::usm_buffer<real_kinds::mystorage, sycl::usm::alloc::shared, false> _a01, _a03, _a05, _a08, _a10, _a12, _a14, _a17,
            _a19;
    bgk::usm_buffer<real_kinds::mystorage, sycl::usm::alloc::shared, false> _b01, _b03, _b05, _b08, _b10, _b12, _b14, _b17,
        _b19;
    #endif
    #else // SYCL_MALLOC_DEVICE
    bgk::usm_buffer<real_kinds::mystorage, sycl::usm::alloc::device> _a01, _a03, _a05, _a08, _a10, _a12, _a14, _a17,
        _a19;
    bgk::usm_buffer<real_kinds::mystorage, sycl::usm::alloc::device> _b01, _b03, _b05, _b08, _b10, _b12, _b14, _b17,
        _b19;
    #endif
    // Preallocate host and device views
    Kokkos::mdspan<real_kinds::mystorage, Kokkos::dextents<std::size_t, 2>, layout> a01_host, a03_host,
        a05_host, a08_host, a10_host, a12_host, a14_host, a17_host, a19_host;
    Kokkos::mdspan<real_kinds::mystorage, Kokkos::dextents<std::size_t, 2>, layout> a01_device, a03_device,
        a05_device, a08_device, a10_device, a12_device, a14_device, a17_device, a19_device;
    Kokkos::mdspan<real_kinds::mystorage, Kokkos::dextents<std::size_t, 2>, layout> b01_host, b03_host,
        b05_host, b08_host, b10_host, b12_host, b14_host, b17_host, b19_host;
    Kokkos::mdspan<real_kinds::mystorage, Kokkos::dextents<std::size_t, 2>, layout> b01_device, b03_device,
        b05_device, b08_device, b10_device, b12_device, b14_device, b17_device, b19_device;

        // std::unique_ptr<real_kinds::mystorage[]> _a01, _a03, _a05, _a08, _a10, _a12, _a14, _a17, _a19;
        // std::unique_ptr<real_kinds::mystorage[]> _b01, _b03, _b05, _b08, _b10, _b12, _b14, _b17, _b19;
        // real_kinds::mystorage* _c01, _c03, _c05, _c08, _c10, _c12, _c14, _c17, _c19;
        // Kokkos::mdspan<real_kinds::mystorage, Kokkos::dextents<std::size_t, 2>, Kokkos::layout_left> a01, a03, a05,
        // a08, a10, a12, a14, a17, a19; Kokkos::mdspan<real_kinds::mystorage, Kokkos::dextents<std::size_t, 2>,
        // Kokkos::layout_left> b01, b03, b05, b08, b10, b12, b14, b17, b19; Kokkos::mdspan<real_kinds::mystorage,
        // Kokkos::dextents<std::size_t, 2>, Kokkos::layout_left> c01, c03, c05, c08, c10, c12, c14, c17, c19;
#else
    // Allocatable arrays for original storage
    // std::vector<std::vector<real_kinds::mystorage>> a01, a03, a05, a08, a10, a12, a14, a17, a19;
    // std::vector<std::vector<real_kinds::mystorage>> b01, b03, b05, b08, b10, b12, b14, b17, b19;
    std::unique_ptr<real_kinds::mystorage[]> _a01, _a03, _a05, _a08, _a10, _a12, _a14, _a17, _a19;
    std::unique_ptr<real_kinds::mystorage[]> _b01, _b03, _b05, _b08, _b10, _b12, _b14, _b17, _b19;
    Kokkos::mdspan<real_kinds::mystorage, Kokkos::dextents<std::size_t, 2>, layout> a01, a03, a05, a08,
        a10, a12, a14, a17, a19;
    Kokkos::mdspan<real_kinds::mystorage, Kokkos::dextents<std::size_t, 2>, layout> b01, b03, b05, b08,
        b10, b12, b14, b17, b19;
#endif

// Obstacle matrix
// std::vector<std::vector<int>> obs;
#ifdef BGK_SYCL_MALLOC_SHARED
    bgk::usm_buffer<int, sycl::usm::alloc::shared> _obs;
#else // SYCL_MALLOC_DEVICE
    bgk::usm_buffer<int, sycl::usm::alloc::device> _obs;
#endif
    Kokkos::mdspan<int, Kokkos::dextents<std::size_t, 2>, layout> obs_host, obs_device;

    int radius;

    // Fluid dynamics variables
    real_kinds::mykind svisc, u0, u00, fgrad;
    real_kinds::mykind cteS; // Smagorinsky constant
    real_kinds::mykind u0x, u0y;
    real_kinds::mykind u_inflow;

    // OpenACC variables
    int mydev, ndev;

    // Correct casting
    real_kinds::mykind omega, omega1;
    static constexpr real_kinds::mykind zero = zero_qp;
    static constexpr real_kinds::mykind uno = uno_qp;
    static constexpr real_kinds::mykind tre = tre_qp;

    static constexpr real_kinds::mykind rf = rf_qp;
    static constexpr real_kinds::mykind qf = qf_qp;
    static constexpr real_kinds::mykind p0 = p0_qp;
    static constexpr real_kinds::mykind p1 = p1_qp;
    static constexpr real_kinds::mykind p2 = p2_qp;


    // Constructor
    storage();
    // storage(std::size_t l, std::size_t ipad, std::size_t m, std::size_t jpad);
    // Init
    void init();

    void update_host();
    void update_device();

    // Utils
    void swap_populations();

  private:
    void swap_population_and_view(auto &src_buffer, auto &src_view_host, auto &src_view_device,
        auto &dest_buffer, auto &dest_view_host, auto &dest_view_device);
};

} // namespace bgk
#endif // __STORAGE_H__
