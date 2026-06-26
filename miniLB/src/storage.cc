#include <get_mem.hh>
#include <limits>
#include <precision.hh>
#include <storage.hh>

namespace bgk {

#ifndef SYCL_DISABLE_CPU_DEVICE
static constexpr auto cpu_selector = sycl::cpu_selector_v;
#else
static constexpr auto cpu_selector = sycl::host_selector{};

#endif

storage::storage()
    : 
      #ifdef DEBUG_HOST_QUEUE
        host_q(std::make_shared<sycl::queue>(sycl::host_selector_v)),
        dev_q(std::make_shared<sycl::queue>(sycl::host_selector_v)),
      #else
        #ifdef SYCL_IN_ORDER_QUEUE
      host_q(std::make_shared<sycl::queue>(cpu_selector, sycl::property::queue::in_order{})),
      dev_q(std::make_shared<sycl::queue>(cpu_selector, sycl::property::queue::in_order{})),
        #else
host_q(std::make_shared<sycl::queue>(cpu_selector)),
      dev_q(std::make_shared<sycl::queue>(cpu_selector)),
        #endif
      #endif
      _a01(host_q, dev_q), _a03(host_q, dev_q),
      _a05(host_q, dev_q), _a08(host_q, dev_q), _a10(host_q, dev_q), _a12(host_q, dev_q), _a14(host_q, dev_q),
      _a17(host_q, dev_q), _a19(host_q, dev_q), _b01(host_q, dev_q), _b03(host_q, dev_q), _b05(host_q, dev_q),
      _b08(host_q, dev_q), _b10(host_q, dev_q), _b12(host_q, dev_q), _b14(host_q, dev_q), _b17(host_q, dev_q),
      _b19(host_q, dev_q), _obs(host_q, dev_q) {}


void storage::init() {
    auto init_lambda = [](auto &usm_buffer, auto &view_host, auto &view_device, const auto row_num, const auto col_num,
                           const auto init_value) {
        // const auto size = (l + 2 + ipad * (m + 2 + jpad));
        const auto size = row_num * col_num;
        usm_buffer.allocate(size);
        std::fill(usm_buffer.m_host_ptr, usm_buffer.m_host_ptr + size, init_value);
        view_host = std::remove_reference_t<decltype(view_host)>(
            usm_buffer.m_host_ptr, Kokkos::dextents<std::size_t, 2>{row_num, col_num});
        view_device = std::remove_reference_t<decltype(view_device)>(
            usm_buffer.m_device_ptr, Kokkos::dextents<std::size_t, 2>{row_num, col_num});
    };

    // Note: To simplify porting porcess, obs has an additional element. In this way, we can directly use the
    // fortran index system (starting from one). This is also useful as the population vectors aXX
    // starts from 0 but they are indexed from 1.
    init_lambda(_obs, obs_host, obs_device, l + 1, m + 1, 0);

    init_lambda(
        _a01, a01_host, a01_device, (l + 2 + ipad), (m + 2 + jpad), std::numeric_limits<real_kinds::mystorage>::max());
    init_lambda(
        _a03, a03_host, a03_device, (l + 2 + ipad), (m + 2 + jpad), std::numeric_limits<real_kinds::mystorage>::max());
    init_lambda(
        _a05, a05_host, a05_device, (l + 2 + ipad), (m + 2 + jpad), std::numeric_limits<real_kinds::mystorage>::max());
    init_lambda(
        _a08, a08_host, a08_device, (l + 2 + ipad), (m + 2 + jpad), std::numeric_limits<real_kinds::mystorage>::max());
    init_lambda(
        _a10, a10_host, a10_device, (l + 2 + ipad), (m + 2 + jpad), std::numeric_limits<real_kinds::mystorage>::max());
    init_lambda(
        _a12, a12_host, a12_device, (l + 2 + ipad), (m + 2 + jpad), std::numeric_limits<real_kinds::mystorage>::max());
    init_lambda(
        _a14, a14_host, a14_device, (l + 2 + ipad), (m + 2 + jpad), std::numeric_limits<real_kinds::mystorage>::max());
    init_lambda(
        _a17, a17_host, a17_device, (l + 2 + ipad), (m + 2 + jpad), std::numeric_limits<real_kinds::mystorage>::max());
    init_lambda(
        _a19, a19_host, a19_device, (l + 2 + ipad), (m + 2 + jpad), std::numeric_limits<real_kinds::mystorage>::max());

    init_lambda(
        _b01, b01_host, b01_device, (l + 2 + ipad), (m + 2 + jpad), std::numeric_limits<real_kinds::mystorage>::max());
    init_lambda(
        _b03, b03_host, b03_device, (l + 2 + ipad), (m + 2 + jpad), std::numeric_limits<real_kinds::mystorage>::max());
    init_lambda(
        _b05, b05_host, b05_device, (l + 2 + ipad), (m + 2 + jpad), std::numeric_limits<real_kinds::mystorage>::max());
    init_lambda(
        _b08, b08_host, b08_device, (l + 2 + ipad), (m + 2 + jpad), std::numeric_limits<real_kinds::mystorage>::max());
    init_lambda(
        _b10, b10_host, b10_device, (l + 2 + ipad), (m + 2 + jpad), std::numeric_limits<real_kinds::mystorage>::max());
    init_lambda(
        _b12, b12_host, b12_device, (l + 2 + ipad), (m + 2 + jpad), std::numeric_limits<real_kinds::mystorage>::max());
    init_lambda(
        _b14, b14_host, b14_device, (l + 2 + ipad), (m + 2 + jpad), std::numeric_limits<real_kinds::mystorage>::max());
    init_lambda(
        _b17, b17_host, b17_device, (l + 2 + ipad), (m + 2 + jpad), std::numeric_limits<real_kinds::mystorage>::max());
    init_lambda(
        _b19, b19_host, b19_device, (l + 2 + ipad), (m + 2 + jpad), std::numeric_limits<real_kinds::mystorage>::max());
}

void storage::update_host() {
    // dev_q->wait();

    _a01.unsafe_update_host();
    _a03.unsafe_update_host();
    _a05.unsafe_update_host();
    _a08.unsafe_update_host();
    _a10.unsafe_update_host();
    _a12.unsafe_update_host();
    _a14.unsafe_update_host();
    _a17.unsafe_update_host();
    _a19.unsafe_update_host();

    _b01.unsafe_update_host();
    _b03.unsafe_update_host();
    _b05.unsafe_update_host();
    _b08.unsafe_update_host();
    _b10.unsafe_update_host();
    _b12.unsafe_update_host();
    _b14.unsafe_update_host();
    _b17.unsafe_update_host();
    _b19.unsafe_update_host();


    dev_q->wait(); //TODO: CHANGE WITH MORE FINE GRAINED WAIT
    #ifdef BKG_SYCL_MALLOC_SHARED
    host_q->wait(); //TODO: CHANGE WITH MORE FINE GRAINED WAIT
    #endif
}

void storage::update_device() {
    // host_q->wait(); //TODO: CHANGE WITH MORE FINE GRAINED WAIT

    _a01.unsafe_update_device();
    _a03.unsafe_update_device();
    _a05.unsafe_update_device();
    _a08.unsafe_update_device();
    _a10.unsafe_update_device();
    _a12.unsafe_update_device();
    _a14.unsafe_update_device();
    _a17.unsafe_update_device();
    _a19.unsafe_update_device();

    _b01.unsafe_update_device();
    _b03.unsafe_update_device();
    _b05.unsafe_update_device();
    _b08.unsafe_update_device();
    _b10.unsafe_update_device();
    _b12.unsafe_update_device();
    _b14.unsafe_update_device();
    _b17.unsafe_update_device();
    _b19.unsafe_update_device();

    _obs.unsafe_update_device();

    dev_q->wait_and_throw(); //TODO: change with more fine grained wait
}

void storage::swap_populations() {
    swap_population_and_view(_a01, a01_host, a01_device, _b01, b01_host, b01_device);
    swap_population_and_view(_a03, a03_host, a03_device, _b03, b03_host, b03_device);
    swap_population_and_view(_a05, a05_host, a05_device, _b05, b05_host, b05_device);
    swap_population_and_view(_a08, a08_host, a08_device, _b08, b08_host, b08_device);
    swap_population_and_view(_a10, a10_host, a10_device, _b10, b10_host, b10_device);
    swap_population_and_view(_a12, a12_host, a12_device, _b12, b12_host, b12_device);
    swap_population_and_view(_a14, a14_host, a14_device, _b14, b14_host, b14_device);
    swap_population_and_view(_a17, a17_host, a17_device, _b17, b17_host, b17_device);
    // TODO: Commented in the original code, need to ask why
    // swap_population_and_view(_a19, a19_host, a19_device, _b19, b19_host, b19_device);
}


void storage::swap_population_and_view(auto &src_buffer, auto &src_view_host, auto &src_view_device, auto &dest_buffer,
    auto &dest_view_host, auto &dest_view_device) {
    swap(src_buffer, dest_buffer);
    swap(src_view_host, dest_view_host);
    swap(src_view_device, dest_view_device);
}

} // namespace bgk