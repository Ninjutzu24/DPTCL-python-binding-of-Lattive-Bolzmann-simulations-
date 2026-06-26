!=====================================================================
!     ****** LBE/draglift
!
!     COPYRIGHT
!       (c) 2000-2011 by CASPUR/G.Amati
!       (c) 2013-20?? by CINECA/G.Amati
!     NAME
!       draglift
!     DESCRIPTION
!       computing total drag/lift  in "easy way" around a box with the obstacle
!     INPUTS
!       itime
!     OUTPUT
!     TODO
!
!     NOTES
!       drag/lift on file drag.lift (unit=66)
!       verify that the obstacle is inside the box       
!
!     *****
!=====================================================================
!
        subroutine draglift(itime)
!
        use storage
        use timing
        implicit none
!
        integer             :: i,j
        integer, INTENT(in) :: itime
        integer             :: istart,istop
        integer             :: jstart,jstop
        integer             :: icoord, jcoord, delta
!
        real(mykind) :: forceX, forceY, norm
        real(mykind) :: force01, force03, force05, force08
        real(mykind) :: force10, force12, force14, force17
!
! cylinder center
        icoord = int(2.0*l/5.0)
        jcoord = int(m/2.0)
! 
! border 
        delta = 3
!
! bounding box
        istart = icoord - radius - delta
        istop  = icoord + radius + delta
        jstart = jcoord - radius - delta
        jstop  = jcoord + radius + delta
!          
! computing drag (force along x) 
        forceX = zero
!
#ifdef OFFLOAD
!$omp target update from(a01,a03,a05,a08,a10,a12,a14,a17,a19)
#endif
!
        do j = jstart, jstop
           do i = istart, istop
              if(obs(i,j)==0) then
                 force01 = 2.0*cx(01)*a01(i,j)*obs(i+icx(01),j+icy(01))
                 force03 = 2.0*cx(03)*a03(i,j)*obs(i+icx(03),j+icy(03))
                 force05 = 2.0*cx(05)*a05(i,j)*obs(i+icx(05),j+icy(05))
                 force08 = 2.0*cx(08)*a08(i,j)*obs(i+icx(08),j+icy(08))
                 force10 = 2.0*cx(10)*a10(i,j)*obs(i+icx(10),j+icy(10))
                 force12 = 2.0*cx(12)*a12(i,j)*obs(i+icx(12),j+icy(12))
                 force14 = 2.0*cx(14)*a14(i,j)*obs(i+icx(14),j+icy(14))
                 force17 = 2.0*cx(17)*a17(i,j)*obs(i+icx(17),j+icy(17))
!                                
                 forceX = forceX + ( force01+force03+force05+force08  & 
                                    +force10+force12+force14+force17)
             endif 
           enddo
        enddo
!
! computing lift (force along y) 
        forceY = zero
        do j = jstart, jstop
           do i = istart, istop
              if(obs(i,j)==0) then
                 force01 = 2.0*cy(01)*a01(i,j)*obs(i+icx(01),j+icy(01))
                 force03 = 2.0*cy(03)*a03(i,j)*obs(i+icx(03),j+icy(03))
                 force05 = 2.0*cy(05)*a05(i,j)*obs(i+icx(05),j+icy(05))
                 force08 = 2.0*cy(08)*a08(i,j)*obs(i+icx(08),j+icy(08))
                 force10 = 2.0*cy(10)*a10(i,j)*obs(i+icx(10),j+icy(10))
                 force12 = 2.0*cy(12)*a12(i,j)*obs(i+icx(12),j+icy(12))
                 force14 = 2.0*cy(14)*a14(i,j)*obs(i+icx(14),j+icy(14))
                 force17 = 2.0*cy(17)*a17(i,j)*obs(i+icx(17),j+icy(17))
!                 
                 forceY = forceY + ( force01+force03+force05+force08  &
                                    +force10+force12+force14+force17)
             endif
           enddo
        enddo

        norm = uno/(u_inflow*u_inflow*radius)
        write(66,*) itime, forceX*norm, forceY*norm
!
        call flush(66)            ! flush for drag/lift
!
#ifdef DEBUG_2
        if(myrank == 0) then
              write(6,*) "DEBUG2: Exiting from sub. drag2", itime
              write(6,*) "DEBUG2", forceX, forceY, norm
        endif
#endif
!
        end subroutine draglift
