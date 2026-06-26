#include <hencol.hh>
#include <iostream>
#include <file_manager.hh>

namespace bgk {
void hencol(storage &bgk_storage) {
    auto& file_manager = debug::file_manager::instance();
    bgk_storage.omega = (storage::uno + storage::uno) / ((6.0 * bgk_storage.svisc) + 1.0);
    bgk_storage.omega1 = (storage::uno + storage::uno) / ((6.0 * bgk_storage.svisc) + 1.0);

#ifdef HALF_P
#ifdef MIXEDPRECISION
    std::cout << "WARNING: Hand-made forcing..." << "\n";
    file_manager.write(16, "WARNING: Hand-made forcing...\n");
    bgk_storage.u0 = 0.0;
    // svisc = 0.05;
    // omega = 1.53846153846153846153;
    std::cout << "WARNING: " << bgk_storage.omega << ", " << bgk_storage.u0 << ", " << bgk_storage.svisc << "\n";
    file_manager.write(16, "WARNING: ", bgk_storage.omega,", ",bgk_storage.u0,", ",bgk_storage.svisc, "\n");
#else
    // nothing to do
#endif
#endif

    bgk_storage.fgrad = 4.0 * bgk_storage.u0 * bgk_storage.svisc / (3.0 * static_cast<real_kinds::mykind>(bgk_storage.m) * static_cast<real_kinds::mykind>(bgk_storage.m));

    if(bgk_storage.myrank == 0) {
        if(bgk_storage.fgrad <= 0.0000001) {
            std::cout << "WARNING: volume forcing below 10e-7" << "\n";
            file_manager.write(16, "WARNING: volume forcing below 10e-7\n");
        }
    }

#ifdef DEBUG_1
    if(bgk_storage.myrank == 0) {
        std::cout << "DEBUG1: omega, fgrad" << bgk_storage.omega << ", " << bgk_storage.fgrad << "\n";
        std::cout << "DEBUG1: Exiting from sub. hencol" << "\n";
    }
#endif
}
} // namespace bgk