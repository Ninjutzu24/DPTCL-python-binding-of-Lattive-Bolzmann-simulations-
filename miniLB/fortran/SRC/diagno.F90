!=====================================================================
!     ****** LBE/diagno
!
!     COPYRIGHT
!       (c) 2000-2011 by CASPUR/G.Amati
!       (c) 2013-20?? by CINECA/G.Amati
!     NAME
!       diagno
!     DESCRIPTION
!       diagnostic subroutine:
!       check of conserved quantities and mean value
!       write on unit 63 (diagno.dat)
!       write on unit 16 (bgk.log)
!     INPUTS
!       itime --> timestep
!     OUTPUT
!       none
!     TODO
!
!     NOTES
!       integer variables used: i,j
!       real variables used: rtot,xtot,stot
!                            xj, rho, rhoinv, rdv
!
!     *****
!=====================================================================
!
       subroutine diagno(itime)
!
       use storage
       implicit none
!
       integer, INTENT(in) :: itime
       integer             :: i,j
!
       real(mykind):: rtot,xtot,ytot,stot
       real(mykind):: xj, yj, rho, rhoinv, rdv
       real(mykind):: x01,x03,x05,x08,x10
       real(mykind):: x12,x14,x17,x19
       real(mykind):: cte1
!
#ifdef NOSHIFT
       cte1 = zero
#else
       cte1 = uno
#endif
!
       rdv = uno/(real(l,mykind)*real(m,mykind))
!
       rtot = zero
       xtot = zero
       ytot = zero
       stot = zero
!
       !
#ifdef NOMANAGED
!$acc update self(a01,a03,a05,a08,a10,a12,a14,a17,a19)
#endif
!
#ifdef OFFLOAD
!$omp target update from(a01,a03,a05,a08,a10,a12,a14,a17,a19)
#endif
       do j=1,m
          do i=1,l
!
             x01 = a01(i,j)
             x03 = a03(i,j)
             x05 = a05(i,j)
             x08 = a08(i,j)
             x10 = a10(i,j)
             x12 = a12(i,j)
             x14 = a14(i,j)
             x17 = a17(i,j)
             x19 = a19(i,j)

! standard form
!                   rho = (+a01(i,j)+a03(i,j)+a05(i,j) &
!                          +a08(i,j)+a10(i,j)+a12(i,j) &
!                          +a14(i,j)+a17(i,j)+a19(i,j)) + cte1
!
! better for half precision????
              rho =(((+x01+x03+x10+x12)+ & 
                     (+x05+x08+x14+x17))+x19)+cte1
!
              rhoinv = uno/rho
!
              xj = ((x03-x12)+(x01-x10)+(x05-x14))*rhoinv
              yj = ((x03-x01)+(x12-x10)+(x08-x17))*rhoinv
!
              rtot = rtot+rho
              xtot = xtot+xj
              ytot = ytot+yj
              stot = stot+(xj*xj+yj*yj)
!
          enddo
       enddo
!
       rtot = (rtot/float(l))/float(m)
       xtot = (xtot/float(l))/float(m)
       ytot = (ytot/float(l))/float(m)
       stot = (stot/float(l))/float(m)
!
       write(16,1001) itime
       write(16,1002) rtot
       write(16,1003) xtot,ytot,stot
       flush(16)
!       
       write(63,1004) itime, xtot, ytot, rtot, stot
       flush(63)
!
#ifdef DEBUG_1
       if (myrank == 0) then
          write(6,*) "DEBUG1: Exiting from sub. diagno"
       endif
#endif
!
! formats...
!
1001   format(" Timestep ",i8)
1002   format("       mean rho ",1(e14.6,1x))
1003   format("       mean vel ",3(e14.6,1x))
1004   format(i8,4(e14.6,1x))
!
       end subroutine diagno
