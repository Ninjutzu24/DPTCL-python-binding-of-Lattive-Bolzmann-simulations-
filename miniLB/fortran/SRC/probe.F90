!=====================================================================
!     ****** LBE/probe
!
!     COPYRIGHT
!       (c) 2000-2011 by CASPUR/G.Amati
!       (c) 2013-20?? by CINECA/G.Amati
!     NAME
!       probe
!     DESCRIPTION
!       Diagnostic subroutine:
!       probe popolations and/or velocities in one single point	
!       write on unit 68 (probe.dat)
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
!       real variables used: rto,xj
!
!     *****
!=====================================================================
!
        subroutine probe(itime,i0,j0)
!
        use storage
        implicit none
!
        integer, INTENT(in) :: i0,j0,itime
!
        real(mykind) :: rho, xj, yj
        real(mykind) :: x01,x03,x05,x08,x10
        real(mykind) :: x12,x14,x17,x19
        real(mykind) :: cte1
!
#ifdef NOSHIFT
       cte1 = zero
#else
       cte1 = uno
#endif
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

        rho = (+x01 +x03 +x05 +x08 +x10 +x12 +x14 +x17 +x19) + cte1

        xj = (x01+x03+x05-x10-x12-x14)
        yj = (x03+x08+x12-x01-x10-x17)

!
       write(68,1002) itime, xj/rho, yj/rho, rho
!
#ifdef DEBUG_2
      if(myrank == 0) then
         write(6,*) "DEBUG2: Exiting from sub. probe", i0,j0
      endif
#endif
!
! format
1002    format(i8,3(e14.6,1x))
!
       end subroutine probe
