#include <get_mem.hh>

double get_mem()
{
    struct rusage usg2;       // for memory leaks..
    double mem;
//    long time;
#ifdef PGI
// do nothing
    mem = 0;
#else
    getrusage(RUSAGE_SELF,&usg2);
    mem = ((double)usg2.ru_maxrss)/1024;
//    time = usg2.ru_utime.tv_sec;
//    fprintf(stdout," time --> %ld\n ",time);
#endif
    return mem;
}