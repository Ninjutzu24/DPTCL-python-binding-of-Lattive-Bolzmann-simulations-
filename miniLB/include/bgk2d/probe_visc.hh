#ifndef __PROBE_VISC_H__
#define __PROBE_VISC_H__

#include <storage.hh>

namespace bgk{
    void probe_visc(storage& bgk_storage, const int itime, const int i0, const int j0);
}

#endif // __PROBE_VISC_H__