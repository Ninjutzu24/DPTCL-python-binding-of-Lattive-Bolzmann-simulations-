#include <kernel_utils.hh>
#include <utils.hh>
#include <fmt/core.h>

namespace bgk{

    void kernel_size_t::get_max_block_size(storage& s){
        auto& q = *s.dev_q;
        // Get max work group size for the current device
        max_block_size = q.get_device().get_info<sycl::info::device::max_work_group_size>();
    }

    kernel_size_t::internal_data kernel_size_t::set_local_size(size_t grid_size) const {
            const auto min_block_size = std::min(grid_size, max_block_size);
            const auto min_block_size_pow_2 = utils::flp2(min_block_size);
            const auto remaining = min_block_size - min_block_size_pow_2;
            const auto grid_size_pow_2 = utils::flp2(grid_size);
            return {grid_size_pow_2, min_block_size_pow_2, remaining};
    }

    kernel_size_t::kernel_size_t(storage& s, int val) {
        get_max_block_size(s);
        sizes = set_local_size(val);
    }

}
