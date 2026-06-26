module real_kinds

!
! not all compilers support half precision
#ifdef HALF_P
  real*2 , parameter :: qq = 1.0
  integer, parameter :: hp = kind(qq)
#endif
  integer, parameter :: sp = kind(1.0)
  integer, parameter :: dp = selected_real_kind(2*precision(1.0_sp))
  integer, parameter :: qp = selected_real_kind(2*precision(1.0_dp))
!
#ifdef DOUBLE_P
  integer, parameter :: mystorage = dp
# ifdef MIXEDPRECISION
  integer, parameter :: mykind = qp
# else
  integer, parameter :: mykind = mystorage
# endif
#else
# ifdef QUAD_P
  integer, parameter :: mystorage = qp
#  ifdef MIXEDPRECISION
   write(6,*) "ERROR :: not implemented"
   stop
#  else
   integer, parameter :: mykind = mystorage
#  endif
# else
#  ifdef HALF_P
   integer, parameter :: mystorage = hp
#   ifdef MIXEDPRECISION
    integer, parameter :: mykind = sp
#   else
    integer, parameter :: mykind = mystorage
#   endif
#  else
! default value (single precision)
  integer, parameter :: mystorage = sp
#   ifdef MIXEDPRECISION
    integer, parameter :: mykind = dp
#   else
    integer, parameter :: mykind = mystorage
#   endif
#  endif
# endif
#endif

end module real_kinds
