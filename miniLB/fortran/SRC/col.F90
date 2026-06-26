!====================================================
!     ****** LBE/coll
!
!     COPYRIGHT
!       (c) 2000-2011 by CASPUR/G.Amati
!       (c) 2013-20?? by CINECA/G.Amati
!     NAME
!       coll
!     DESCRIPTION
!       Collision according to bgk style (\omega(f_f^(eq)))
!       forcing is included and is proportional to u_0
!     INPUTS
!       itime --> timestep 
!       none
!     TODO
!
!     NOTES
!       integer variables used: i,k,itime
!       real variables used: 
!
!
!     *****
!====================================================
!
        subroutine col(itime)
!
        use storage
        use real_kinds
        use timing
!
        implicit none
!
        integer, intent(IN) :: itime
        integer             :: i,j
!
        real(mykind) :: x01,x03,x05,x08,x10
        real(mykind) :: x12,x14,x17,x19
        real(mykind) :: e01,e03,e05,e08,e10
        real(mykind) :: e12,e14,e17,e19
        real(mykind) :: n01,n03,n05,n08,n10
        real(mykind) :: n12,n14,n17,n19
        real(mykind) :: rho,rhoinv,vx,vy,vx2,vy2,vsq
        real(mykind) :: vxpy,vxmy,rp1,rp2,rp0
        real(mykind) :: qxpy,qxmy,qx,qy,q0
        real(mykind) :: forcex, forcey
        real(mykind) :: cte1,cte0
        real(mykind) :: Pxx,Pxy,Pyx,Pyy,Ptotal
        real(mykind) :: Ts
!
#ifdef DEBUG_3
        real(mykind) :: cte
        character(len=17) :: file_nameD
        file_nameD = 'debug.xxx.xxx.log'
        write(file_nameD(7:9),3300) itime
        write(file_nameD(11:13),3300) myrank
        open(41,file=file_nameD, status='unknown')        ! debug file
!
!        call probe(itime,(3*l/4),(m/2))
#endif
!
#ifdef NOSHIFT
        cte1 = zero
#else
        cte1 = uno
#endif
        cte0 = uno - cte1
!
! initialize constant.....        
        forcex = zero
        forcey = zero
!
#ifdef OFFLOAD
!$OMP target teams distribute parallel do simd collapse(2)
        do j = 1,m
           do i = 1,l
#elif OPENACC
#ifdef KERNELS
 !$acc kernels
 !$acc loop independent collapse(2)
#else
 !$acc parallel
 !$acc loop independent collapse(2)
#endif
        do j = 1,m
           do i = 1,l
#else
         do concurrent (j=1:m, i=1:l)
#endif
!
           x01 = b01(i,j)
           x03 = b03(i,j)
           x05 = b05(i,j)
           x08 = b08(i,j)
           x10 = b10(i,j)
           x12 = b12(i,j)
           x14 = b14(i,j)
           x17 = b17(i,j)
           x19 = a19(i,j)
!
! natural way of expressing rho
!           rho = (x01+x03+x05+x08+x10+x12+x14+x17+x19)+cte1
!
! better for half precision
           rho = (((x01+x03+x10+x12)+(x05+x08+x14+x17))+x19)+cte1
!
           rhoinv = uno/rho
!
           vx = ((x03-x12)+(x01-x10)+(x05-x14))*rhoinv
           vy = ((x03-x01)+(x12-x10)+(x08-x17))*rhoinv
!
#ifdef DEBUG_3
        write(41,3131) i+offset(1),j+offset(2), obs(i,j),   & 
                        vx,vy,rho
!                       x01,x03,x05,x08,x10,x12,x14,x17,x19
#endif
!
! Quadratic terms
           vx2 = vx*vx
           vy2 = vy*vy
!
           vsq = vx2+vy2
!
           vxpy = vx+vy
           vxmy = vx-vy
!              
           qxpy = cte0+qf*(tre*vxpy*vxpy-vsq)
           qxmy = cte0+qf*(tre*vxmy*vxmy-vsq)
           qx   = cte0+qf*(tre*vx2      -vsq)
           qy   = cte0+qf*(tre*vy2      -vsq)
           q0   = cte0+qf*(             -vsq)
!
! linear terms
           vx   = rf*vx
           vy   = rf*vy
           vxpy = rf*vxpy
           vxmy = rf*vxmy
!
! constant terms
           rp0 = rho*p0
           rp1 = rho*p1
           rp2 = rho*p2
!
! equilibrium distribution
           e01 = rp2*(+vxmy+qxmy)+cte1*(rp2-p2)
           e03 = rp2*(+vxpy+qxpy)+cte1*(rp2-p2)
           e05 = rp1*(+vx  +qx  )+cte1*(rp1-p1)
           e08 = rp1*(+vy  +qy  )+cte1*(rp1-p1)
           e10 = rp2*(-vxpy+qxpy)+cte1*(rp2-p2)
           e12 = rp2*(-vxmy+qxmy)+cte1*(rp2-p2)
           e14 = rp1*(-vx  +qx  )+cte1*(rp1-p1)
           e17 = rp1*(-vy  +qy  )+cte1*(rp1-p1)
           e19 = rp0*(     +q0  )+cte1*(rp0-p0)
!           
#ifdef LES           
! compute les           
!           
!non-equilibrium distribution
           n01 = x01-e01
           n03 = x03-e03
           n05 = x05-e05
           n08 = x08-e08
           n10 = x10-e10
           n12 = x12-e12
           n14 = x14-e14
           n17 = x17-e17
           n19 = x19-e19
!
! compute Pij           
           Pxx = cx(01)*cx(01)*n01 + &
                 cx(03)*cx(03)*n03 + &
                 cx(05)*cx(05)*n05 + &
                 cx(08)*cx(08)*n08 + &
                 cx(10)*cx(10)*n10 + &
                 cx(12)*cx(12)*n12 + &
                 cx(14)*cx(14)*n14 + &
                 cx(17)*cx(17)*n17 + &
                 cx(19)*cx(19)*n19
!
           Pyy = cy(01)*cy(01)*n01 + &
                 cy(03)*cy(03)*n03 + &
                 cy(05)*cy(05)*n05 + &
                 cy(08)*cy(08)*n08 + &
                 cy(10)*cy(10)*n10 + &
                 cy(12)*cy(12)*n12 + &
                 cy(14)*cy(14)*n14 + &
                 cy(17)*cy(17)*n17 + &
                 cy(19)*cy(19)*n19
!
           Pxy = cx(01)*cy(01)*n01 + &
                 cx(03)*cy(03)*n03 + &
                 cx(05)*cy(05)*n05 + &
                 cx(08)*cy(08)*n08 + &
                 cx(10)*cy(10)*n10 + &
                 cx(12)*cy(12)*n12 + &
                 cx(14)*cy(14)*n14 + &
                 cx(17)*cy(17)*n17 + &
                 cx(19)*cy(19)*n19
!
           Pyx = Pxy
!           
! calculate Pi total
           Ptotal =sqrt((Pxx)**2 + (2*Pxy*Pyx) + (Pyy)**2)
!           
! adding turbulent viscosity
           Ts = 1/(2*omega1) + sqrt(18*(cteS)**2 *Ptotal+(1/omega1)**2)/2
           omega = 1/Ts
!
#endif
!
! forcing term
!
# ifdef CHANNEL
#  ifdef FORCING_Y
           forcey = fgrad*rho
#  else
           forcex = fgrad*rho     ! default value...
#  endif
# endif
!
# ifdef MYVERSION
#  ifdef FORCING_Y
           forcey = fgrad*rho
#  else
           forcex = fgrad*rho     ! default value...
#  endif
# endif
!
! loop on populations
           a01(i,j) = x01 - omega*(x01-e01) + (+ forcex - forcey)
           a03(i,j) = x03 - omega*(x03-e03) + (+ forcex + forcey)
           a05(i,j) = x05 - omega*(x05-e05) + (+ forcex)
           a08(i,j) = x08 - omega*(x08-e08) + (         + forcey)
           a10(i,j) = x10 - omega*(x10-e10) + (- forcex - forcey)
           a12(i,j) = x12 - omega*(x12-e12) + (- forcex + forcey)
           a14(i,j) = x14 - omega*(x14-e14) + (- forcex)
           a17(i,j) = x17 - omega*(x17-e17) + (         - forcey)
           a19(i,j) = x19 - omega*(x19-e19)
!
!           if((i==l/2+1).AND.(j==m)) then  
!!              write(6,*) b01(l/2+1,m), a01(l/2+1,m)
!               write(6,*) x01, omega, e01, forcex, forcey
!               write(6,*) x01 - omega*(x01-e01) 
!               pause
!           endif

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
#ifdef DEBUG_3
!
! format
3300  format(i3.3)
!3131  format(3(i,1x),6(e14.6,1x))
3131  format(3(i,1x),3(e14.6,1x))
        close(41)
#endif
!
#ifdef DEBUG_2
        if(myrank == 0) then
           write(6,*) "DEBUG2: Exiting from sub. coll", forcex, forcey, &
           b01(l/2+1,m) , a01(l/2+1,m)

        endif
#endif
        end subroutine col
