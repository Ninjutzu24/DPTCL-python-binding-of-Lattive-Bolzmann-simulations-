!=======================================================================
!     ****** LBE/bcond_obs
!
!     COPYRIGHT
!       (c) 2000-2011 by CASPUR/G.Amati
!       (c) 2013-20?? by CINECA/G.Amati
!     NAME
!       bcond_obs
!     DESCRIPTION
!       obstcle
!     INPUTS
!       
!     OUTPUT
!       none
!     TODO
!
!     NOTES
!     *****
!=======================================================================
!
        subroutine bcond_obs
!
        use timing
        use storage
!
        implicit none
!
        integer:: i, j 
!
        call SYSTEM_CLOCK(countO0, count_rate, count_max)
        call time(tcountO0)
!
#ifdef OFFLOAD
!$OMP target teams distribute parallel do simd collapse(2)
        do j=jmin,jmax
        do i=imin,imax
#elif OPENACC
#ifdef KERNELS
 !$acc kernels
 !$acc loop independent collapse(2)
#else
 !$acc parallel
 !$acc loop independent collapse(2)
#endif
        do j=jmin,jmax
        do i=imin,imax
#else
        do concurrent (j=jmin:jmax, i=imin:imax)
#endif
           if(obs(i,j)==1) then
              a01(i,j) = a12(i+1,j-1)
              a03(i,j) = a10(i+1,j+1)
              a05(i,j) = a14(i+1,j  )
              a08(i,j) = a17(i  ,j+1)
              a10(i,j) = a03(i-1,j-1)
              a12(i,j) = a01(i-1,j+1)
              a14(i,j) = a05(i-1,j  )
              a17(i,j) = a08(i  ,j-1)
           endif
        end do
#ifdef OFFLOAD
        end do
!$OMP end target teams distribute parallel do simd
#elif OPENACC
        end do
#ifdef KERNELS
        !$acc end kernels
#else
        !$acc end parallel
#endif
#endif

!
! stop timing
        call time(tcountO1)
        call SYSTEM_CLOCK(countO1, count_rate, count_max)
        time_obs = time_obs + real(countO1-countO0)/(count_rate)
        time_obs1 = time_obs1 + (tcountO1-tcountO0)
!
#ifdef DEBUG_2
        if(myrank == 0) then
           write(6,*) "DEBUG2: Exiting from sub. bcond_obs", &
                               imin,imax,jmin,imax
           write(6,*) " "
        endif
#endif
!
        end subroutine bcond_obs
