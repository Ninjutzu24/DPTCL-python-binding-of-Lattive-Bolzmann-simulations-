#ifndef __PRECISION_H__
#define __PRECISION_H__

#include<stdint.h>

namespace bgk::real_kinds {

#ifdef HALF_P
    using hp = uint16_t; // Placeholder for half precision
#endif

    using sp = float;
    using dp = double;
    using qp = long double; // Note: Precision varies by implementation

#ifdef DOUBLE_P
    using mystorage = dp;
    #ifdef MIXEDPRECISION
        using mykind = qp;
    #else
        using mykind = mystorage;
    #endif
#else
    #ifdef QUAD_P
        using mystorage = qp;
        #ifdef MIXEDPRECISION
            // Error handling for not implemented case
            static_assert(false, "ERROR :: MIXEDPRECISION with QUAD_P is not implemented");
        #else
            using mykind = mystorage;
        #endif
    #else
        #ifdef HALF_P
            using mystorage = hp;
            #ifdef MIXEDPRECISION
                using mykind = sp;
            #else
                using mykind = mystorage;
            #endif
        #else
            // Default value (single precision)
            using mystorage = sp;
            #ifdef MIXEDPRECISION
                using mykind = dp;
            #else
                using mykind = mystorage;
            #endif
        #endif
    #endif
#endif

} // namespace real_kinds
#endif // __PRECISION_H__
