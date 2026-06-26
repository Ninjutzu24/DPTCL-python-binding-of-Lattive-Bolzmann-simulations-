#ifndef __INITIALIZE_H__
#define __INITIALIZE_H__

#include <storage.hh>

namespace bgk{

void initialize(storage &bgk_storage, const int itrestart, const int init_v, const int itfin, int &itstart,
    const int ivtim, const int isignal, const int itsave, const int icheck);

}

#endif // __INITIALIZE_H__