!=====================================================================
!     ****** LBE/probe_visc
!
!     COPYRIGHT
!       (c) 2000-2011 by CASPUR/G.Amati
!       (c) 2013-20?? by CINECA/G.Amati
!     NAME
!       probe
!     DESCRIPTION
!       Diagnostic subroutine:
!       probe viscosity for LES in one single point	
!       write on unit 65 (probe.dat)
!     INPUTS
!       itime --> timestep
!       i0     --> x coordinate
!       j0     --> y coordinate
!     OUTPUT
!       none
!     TODO
!
!     NOTES
!       integer variables used: i,k,itime
!       real variables used: rho,xj
!
!     *****
!=====================================================================
!
        subroutine probe_visc(itime,i0,j0)
!
        use storage
        implicit none
!
        integer, INTENT(in) :: i0,j0,itime
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
        real(mykind) :: Pxx,Pxy,Pyx,Pyy,Ptotal
        real(mykind) :: Ts
        real(mykind) :: cte1, cte0

!
#ifdef NOSHIFT
        cte1 = zero
#else
        cte1 = uno
#endif
        cte0 = uno - cte1
!
        x01 = a01(i0,j0)
        x03 = a03(i0,j0)
        x05 = a05(i0,j0)
        x08 = a08(i0,j0)
        x10 = a10(i0,j0)
        x12 = a12(i0,j0)
        x14 = a14(i0,j0)
        x17 = a17(i0,j0)
        x19 = a19(i0,j0)
!
        rho = (+x01 +x03 +x05 +x08 +x10 +x12 +x14 +x17 +x19) + cte1
!
        vx = (x01+x03+x05-x10-x12-x14)/rho
        vy = (x03+x08+x12-x01-x10-x17)/rho
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
! non-equilibrium distribution
        n01 = x01-e01
        n03 = x03-e03
        n05 = x05-e05
        n08 = x08-e08
        n10 = x10-e10
        n12 = x12-e12
        n14 = x14-e14
        n17 = x17-e17
!
        Pxx = n01 + &
              n03 + &
              n05 + &
              n10 + &
              n12 + &
              n14
!
        Pyy = n01 + &
              n03 + &
              n08 + &
              n10 + &
              n12 + &
              n17
!
        Pxy = -n01+n03+n10-n12
!
        Pyx = Pxy
!
! calculate Pi total
        Ptotal =sqrt((Pxx)**2 + (2*Pxy*Pyx) + (Pyy)**2)
!
! adding turbulent viscosity
        Ts = 1/(2*omega1) + sqrt(18.0*(cteS**2)*Ptotal + (1.0/omega1)**2)/2
        omega = 1/Ts

        write(65,1002) itime,           &
                       (2/omega1 -1)/6, &
                       (2/omega -1)/6
!
#ifdef DEBUG_2
      if(myrank == 0) then
         write(6,*) "DEBUG2: Exiting from sub. probe", i0,j0
      endif
#endif
!
! format
1002    format(i8,2(e14.6,1x))
!
       end subroutine probe_visc
