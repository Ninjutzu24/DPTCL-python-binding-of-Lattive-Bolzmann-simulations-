#ifndef __OUTDAT_H__
#define __OUTDAT_H__

#include <storage.hh>

namespace bgk{

    void outdat(storage& bgk_storage, const int itfin,const int itstart,const int ivtim,const int isignal,const int itsave,const int icheck);
}

#endif // __OUTDAT_H__