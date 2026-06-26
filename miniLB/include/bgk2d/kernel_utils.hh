#include <storage.hh>
#include <sycl/sycl.hpp>

namespace bgk{

struct kernel_size_t{
    
    struct internal_data{
        size_t grid_size;
        size_t block_size;
        size_t remaining;
    };
    
    internal_data sizes;
    size_t max_block_size;

    void get_max_block_size(storage& s);
    internal_data set_local_size(size_t grid_size) const;

    kernel_size_t(storage& s, int val);
};

}