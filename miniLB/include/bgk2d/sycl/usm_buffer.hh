#include <sycl/sycl.hpp>

namespace bgk {

template<typename T, sycl::usm::alloc kind, bool prefetch = false>
struct usm_buffer {
    using value_type = T;
    using pointer = T *;
    using const_pointer = const T *;
    using reference = T &;

    usm_buffer(const std::shared_ptr<sycl::queue> &host_q, const std::shared_ptr<sycl::queue> &dev_q)
        : m_host_q(host_q), m_dev_q(dev_q) {}

    void allocate(size_t size) {
        m_size = size;
        if constexpr(kind == sycl::usm::alloc::shared) {
            m_host_ptr = m_device_ptr = sycl::malloc_shared<T>(size, *m_dev_q);
        } else if constexpr(kind == sycl::usm::alloc::device) {
            m_device_ptr = sycl::malloc_device<T>(size, *m_dev_q);
            m_host_ptr = sycl::malloc_host<T>(size, *m_dev_q);
        }
    }

    ~usm_buffer() {
        if constexpr(kind == sycl::usm::alloc::shared) {
            sycl::free(m_host_ptr, *m_dev_q);
        } else if(kind == sycl::usm::alloc::device) {
            sycl::free(m_device_ptr, *m_dev_q);
            sycl::free(m_host_ptr, *m_dev_q);
        }
    }

    // Wait for all operations to finish and copy the data
    // Every kernel uses the same elements, so we're not loosing parallelism
    void update_host() {
        m_dev_q->wait();
        if constexpr(kind == sycl::usm::alloc::device) {
            m_dev_q->copy(m_device_ptr, m_host_ptr, m_size).wait();
        } else if constexpr(prefetch) {
            m_host_q->prefetch(m_device_ptr, m_size);
        }
    }

    // Update host without waiting for operations to finish
    // This is useful when we need to update multiple buffers at the same time
    void unsafe_update_host() {
        if constexpr(kind == sycl::usm::alloc::device) { m_dev_q->copy(m_device_ptr, m_host_ptr, m_size).wait(); }
        else if constexpr (prefetch){
            m_host_q->prefetch(m_device_ptr, m_size);
        }
    }

    // Wait for all operations to finish and copy the data
    // Every kernel uses the same elements, so we're not loosing parallelism
    void update_device() {
        m_host_q->wait();
        if constexpr(kind == sycl::usm::alloc::device) { m_dev_q->copy(m_host_ptr, m_device_ptr, m_size).wait(); }
        else if constexpr (prefetch){
            m_dev_q->prefetch(m_host_ptr, m_size);
        }
    }

    // Update device without waiting for operations to finish
    // This is useful when we need to update multiple buffers at the same time
    void unsafe_update_device() {
        if constexpr(kind == sycl::usm::alloc::device) { m_dev_q->copy(m_host_ptr, m_device_ptr, m_size).wait(); }
        else if constexpr (prefetch){
            m_dev_q->prefetch(m_host_ptr, m_size);
        }
    }

    friend void swap(usm_buffer<T, kind> &first, usm_buffer<T, kind> &other) {
        using std::swap;
        swap(first.m_host_ptr, other.m_host_ptr);
        swap(first.m_device_ptr, other.m_device_ptr);
        swap(first.m_size, other.m_size);
        // This is not really necessary, there is just one queue in the system...
        swap(first.m_host_q, other.m_host_q);
        swap(first.m_dev_q, other.m_dev_q);
    }

    T *m_host_ptr;
    T *m_device_ptr;
    size_t m_size;
    std::shared_ptr<sycl::queue> m_host_q;
    std::shared_ptr<sycl::queue> m_dev_q;
};

} // namespace bgk