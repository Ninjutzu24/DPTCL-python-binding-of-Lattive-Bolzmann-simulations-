!=====================================================================
!     ****** LBE/bcond_channel
!
!     COPYRIGHT
!       (c) 2000-2011 by CASPUR/G.Amati
!       (c) 2013-20?? by CINECA/G.Amati
!     NAME
!       bcond_channel: simple (periodic) channel flow with
!                      * periodic (rear)
!                      * periodic (front)
!                      * no slip (left/right)
!     DESCRIPTION
!       2D periodic bc
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
        subroutine bcond_channel
!
        use timing
        use storage
!
        implicit none
!
        integer      :: i,j
        real(mykind) :: cte1
!
#ifdef CHANNEL
!
# ifdef NOSHIFT
       cte1 = uno
# else
       cte1 = zero
# endif

!
! start timing...
        call SYSTEM_CLOCK(countA0, count_rate, count_max)
        call time(tcountA0)
!
! ----------------------------------------------
! loop foused for performance reason (for GPU)
! -------------------------------------------------------------
#ifdef OFFLOAD
!$OMP target teams distribute parallel do simd
        do j=0,m+1
#elif OPENACC
 #ifdef KERNELS
 !$acc kernels
 !$acc loop independent
 #else
 !$acc parallel
 !$acc loop independent
 #endif
        do j=0,m+1
#else
        do concurrent (j=0:m+1)
#endif
!           
! rear, periodic bc  (x = l)
!
           a10(l1,j) = a10(1,j)
           a12(l1,j) = a12(1,j)
           a14(l1,j) = a14(1,j)
!
! -------------------------------------------------------------
! front, periodic (x = 0)
!           
           a01( 0,j) = a01(l,j)
           a03( 0,j) = a03(l,j)
           a05( 0,j) = a05(l,j)
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
! ----------------------------------------------
! left (y = 0)  
! right (y = m) 
! ----------------------------------------------
!
#ifdef OFFLOAD
!$OMP target teams distribute parallel do simd 
        do i=0,l+1
#elif OPENACC
 #ifdef KERNELS
 !$acc kernels
 !$acc loop independent
 #else
 !$acc parallel
 !$acc loop independent
 #endif
        do i=0,l+1
#else
        do concurrent (i=0:l+1)
#endif
! left, noslip  (y = 0)  
           a08(i  ,0)  = a17(i,1)
           a12(i+1,0)  = a01(i,1)
           a03(i-1,0)  = a10(i,1)

! right, noslip  (y = m) 
           a10(i+1,m1) = a03(i,m)
           a17(i  ,m1) = a08(i,m)
           a01(i-1,m1) = a12(i,m) 
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
!
! ----------------------------------------------
! stop timing
        call time(tcountA1)
        call SYSTEM_CLOCK(countA1, count_rate, count_max)
        time_bc = time_bc + real(countA1-countA0)/(count_rate)
        time_bc1 = time_bc1 + (tcountA1-tcountA0)
!
#endif
!
#ifdef DEBUG_2
        if(myrank == 0) then
           write(6,*) "DEBUG2: Exiting from sub. bcond_channel"
        endif
#endif
        end subroutine bcond_channel
