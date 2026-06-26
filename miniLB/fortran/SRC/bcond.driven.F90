!=====================================================================
!     ****** LBE/bcond_driven
!
!     COPYRIGHT
!       (c) 2000-2011 by CASPUR/G.Amati
!       (c) 2013-20?? by CINECA/G.Amati
!     NAME
!       bcond
!     DESCRIPTION
!
!     INPUTS
!       none
!     OUTPUT
!       none
!     TODO
!       
!     NOTES
!       Order of upgrading bc
!       1) front (x = l)
!       2) rear  (x = 0)
!       3) left  (y = 0)
!       4) right (y = m)
!
!     *****
!=====================================================================
!
        subroutine bcond_driven
!
        use timing
        use storage
!
        implicit none
!
        integer      :: i,j 
        real(mykind) :: force
!
! start timing...
        call SYSTEM_CLOCK(countA0, count_rate, count_max)
        call time(tcountA0)
!
        force =  u00/(6.0)
!
#ifdef TRICK1
! it is correct only if l=m

# ifdef OFFLOAD
!$OMP target teams distribute parallel do simd 
        do j=1,m
# elif OPENACC
!$acc kernels
!$acc loop independent
        do j=1,m
# else
        do concurrent (j=1:m)
# endif
! right (y = m) lid-wall
           a01(j-1,m1) = a12(j,m) + force
           a17(j  ,m1) = a08(j,m)
           a10(j+1,m1) = a03(j,m) - force

! front (x = l)
           a12(l1,j-1) = a01(l,j)
           a10(l1,j+1) = a03(l,j)
           a14(l1,j  ) = a05(l,j)

! rear (x = 0)
           a03(0,j-1) = a10(1,j)
           a01(0,j+1) = a12(1,j)
           a05(0,j  ) = a14(1,j)

! left (y = 0)
           a03(j-1,0)  = a10(j,1)
           a08(j  ,0)  = a17(j,1)
           a12(j+1,0)  = a01(j,1)

        end do
# ifdef OFFLOAD
!$OMP end target teams distribute parallel do simd
# elif OPENACC
!$acc end kernels
# endif

#else

#ifdef OFFLOAD
!$OMP target teams distribute parallel do simd 
        do j=1,m
#elif OPENACC
#ifdef KERNELS
 !$acc kernels
 !$acc loop independent
#else
 !$acc parallel
 !$acc loop independent 
#endif
        do j=1,m
#else
        do concurrent (j=1:m)
#endif
! front (x = l)
           a12(l1,j-1) = a01(l,j)
           a10(l1,j+1) = a03(l,j)
           a14(l1,j  ) = a05(l,j)

! rear (x = 0)
           a03(0,j-1) = a10(1,j)
           a01(0,j+1) = a12(1,j)
           a05(0,j  ) = a14(1,j)
        end do
#ifdef OFFLOAD
!$OMP end target teams distribute parallel do simd
#elif OPENACC
#ifdef KERNELS
        !$acc end kernels
#else
        !$acc end parallel
#endif
#endif
!        
#ifdef OFFLOAD
!$OMP target teams distribute parallel do simd 
        do i=1,l
#elif OPENACC
# ifdef KERNELS
 !$acc kernels
 !$acc loop independent
# else
 !$acc parallel
 !$acc loop independent
# endif
        do i=1,l
#else
        do concurrent (i=1:l)
#endif
! left (y = 0)  
           a08(i  ,0)  = a17(i,1)
           a12(i+1,0)  = a01(i,1)
           a03(i-1,0)  = a10(i,1)

! right (y = m) lid-wall
           a10(i+1,m1) = a03(i,m) - force
           a17(i  ,m1) = a08(i,m)
           a01(i-1,m1) = a12(i,m) + force
        enddo
#ifdef OFFLOAD
!$OMP end target teams distribute parallel do simd
#elif OPENACC
#ifdef KERNELS
        !$acc end kernels
#else
        !$acc end parallel
#endif
#endif
        
#endif
!
! stop timing
        call time(tcountA1)
        call SYSTEM_CLOCK(countA1, count_rate, count_max)
        time_bc = time_bc + real(countA1-countA0)/(count_rate)
        time_bc1 = time_bc1 + (tcountA1-tcountA0)
!
#ifdef DEBUG_2
        if(myrank == 0) then
           write(6,*) "DEBUG2: Exiting from sub. bcond_driven"
        endif
#endif
!        
        end subroutine bcond_driven
