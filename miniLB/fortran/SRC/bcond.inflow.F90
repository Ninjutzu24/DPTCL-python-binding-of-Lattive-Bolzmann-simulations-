!=====================================================================
!     ****** LBE/bcond_inflow
!
!     COPYRIGHT
!       (c) 2000-2011 by CASPUR/G.Amati
!       (c) 2013-20?? by CINECA/G.Amati
!     NAME
!       bcond_inflow: simple flow with
!                      * inflow (rear)
!                      * outflow (front)
!                      * periodic b.c. (left/right)
!     DESCRIPTION
!       2D periodic bc
!     INPUTS
!       none
!     OUTPUT
!       none
!     TODO
!       remove hardwritten inflow velocity       
!       
!     NOTES
!       Order of upgrading bc
!       1) front (x = l)
!       2) rear  (x = 0)
!       3) left  (y = 0)
!       4) right (y = m)
!
!       inflow velocity  is hardwritten : 0.1       
!
!     *****
!=====================================================================
!
        subroutine bcond_inflow
!
        use timing
        use storage
!
        implicit none
!
        integer      :: i,j
        real(mykind) :: xj,yj
        real(mykind) :: cvsq,crho, rhoinv
        real(mykind) :: cx01,cx03,cx05
        real(mykind) :: cx10,cx12,cx14
        real(mykind):: cte1
!
!
#ifdef NOSHIFT
       cte1 = uno
#else
       cte1 = zero
#endif

!
! start timing...
        call SYSTEM_CLOCK(countA0, count_rate, count_max)
        call time(tcountA0)
!
        u_inflow=0.1
!        
! ----------------------------------------------
! loop fused for perfomance reason (on GPU)        
! front, outflow  (x = l)
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
!         y = (real(y,mykind) -0.5 - 0.5*real(m,mykind))/(0.5*real(m,mykind))
           crho  =uno
           rhoinv=uno
!           
! front, outflow  (x = l)
           xj = ((a03(l,j)-a12(l,j)) & 
                +(a01(l,j)-a10(l,j)) & 
                +(a05(l,j)-a14(l,j)))*rhoinv
           yj = ((a03(l,j)-a01(l,j)) &
                +(a12(l,j)-a10(l,j)) &
                +(a08(l,j)-a17(l,j)))*rhoinv
!          
           cvsq=xj*xj+yj*yj
!
           cx10 = rf*(-xj-yj)+qf*(3.0*(xj+yj)*(xj+yj)-cvsq)
           cx12 = rf*(-xj+yj)+qf*(3.0*(xj-yj)*(xj-yj)-cvsq)
           cx14 = rf*(-xj   )+qf*(3.0*(xj   )*(xj   )-cvsq)
!
           a10(l1,j) = crho*p2*(cte1+cx10)
           a12(l1,j) = crho*p2*(cte1+cx12)
           a14(l1,j) = crho*p1*(cte1+cx14)

! rear, inflow (x = 0)
           xj = u_inflow
           yj = zero
!           
           cvsq=xj*xj+yj*yj
!
           cx01 = rf*( xj-yj   )+qf*(3.d0*(xj-yj)*(xj-yj)-cvsq)
           cx03 = rf*( xj+yj   )+qf*(3.d0*(xj+yj)*(xj+yj)-cvsq)
           cx05 = rf*( xj      )+qf*(3.d0*(xj   )*(xj   )-cvsq)
!
           a01(0,j) = crho*p2*(cte1+cx01)
           a03(0,j) = crho*p2*(cte1+cx03)
           a05(0,j) = crho*p1*(cte1+cx05)
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
           a17(i,m1) = a17(i,1)
           a01(i,m1) = a01(i,1)
           a10(i,m1) = a10(i,1)

! right, noslip  (y = m) 
           a03(i,0) = a03(i,m)
           a08(i,0) = a08(i,m)
           a12(i,0) = a12(i,m)
        enddo
#ifdef OFFLOAD
!$OMP end target teams distribute parallel do simd
#elif OPENACC
#ifdef KERNELS
!$acc end kernels
#else
!$acc end parallel
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
           write(6,*) "DEBUG2: Exiting from sub. bcond_inflow", & 
           u_inflow
        endif
#endif
!        
        end subroutine bcond_inflow
