!=====================================================================
!     ****** LBE/build_obs
!
!     COPYRIGHT
!       (c) 2000-2011 by CASPUR/G.Amati
!       (c) 2013-20?? by CINECA/G.Amati
!     NAME
!       bcond
!     DESCRIPTION
!       build obstacles: setting 1 in obs(i,j) field 
!     INPUTS
!       none
!     OUTPUT
!       none
!     TODO
!       
!     NOTES
!
!     *****
!=====================================================================
!
      subroutine build_obs
!
      use timing
      use storage
!
      implicit none
!
      integer:: i, j
      integer:: icoord, jcoord
      integer:: itime
      real(mykind):: d2, R2a, R2b, R
      real(mykind):: hsize
      real(mykind):: soglia,a
      real(mykind):: rand
!
      imin = l
      jmin = m
      imax = 0
      jmax = 0
      nobs = 0
!
      myrank = 0
      itime  = 0
!
! creating  obstacle 
!
#ifdef SQUARE
! creating square
      write( 6,*) "INFO: creating obstacle (square)" 
      write(16,*) "INFO: creating obstacle (square)" 
!
! center of the square
      icoord = 2*l/5
      jcoord = m/2
      hsize  = radius  ! half size of the square
!      
      write( 6,*) "INFO: square size   -->", 2*hsize, 2*hsize/m
      write( 6,*) "INFO: Cyl icoord    -->", icoord, icoord/l
      write( 6,*) "INFO: Cyl jcoord    -->", jcoord, jcoord/m
!
      do j = 1, m
         if((j.ge.(jcoord-hsize)).and.(j.le.(jcoord+hsize))) then
            do i = 1, l
               if((i.ge.(icoord-hsize)).and.(i.le.(icoord+hsize))) then
!                    
                   obs(i,j) = 1
                   nobs = nobs + 1
!
                   imin = min(imin,i)
                   jmin = min(jmin,j)
!
                   imax = max(imax,i)
                   jmax = max(jmax,j)
!
               endif
            enddo
         endif
      enddo
#elif POROUS
! creating porous 
      write( 6,*) "INFO: creating obstacle (porous)" 
      write(16,*) "INFO: creating obstacle (porous)" 
!      
! threshold
      soglia=0.4
      write( 6,*) "INFO: threshold (porous)", soglia 
      write(16,*) "INFO: threshold (porous)", soglia 
!      
      do j = 5, m-4, 5
         do i = l/4, 3*l/4, 5
            a = rand()
            if(a.gt.soglia) then
               obs(i+1,j  ) = 1
               obs(i  ,j+1) = 1
               obs(i  ,j  ) = 1
               obs(i-1,j  ) = 1
               obs(i  ,j-1) = 1

               nobs = nobs + 5
!
               imin = min(imin,i-1)
               jmin = min(jmin,j-1)
!
               imax = max(imax,i+1)
               jmax = max(jmax,j+1)
!
            endif
         enddo
      enddo
#else
! creating circle
      write( 6,*) "INFO: creating obstacle (cylinder)" 
      write(16,*) "INFO: creating obstacle (cylinder)" 
!
      icoord = 2*l/5
      jcoord = m/2
!      
      write( 6,*) "INFO: Cyl radius    -->", radius, radius/m
      write( 6,*) "INFO: Cyl icoord    -->", icoord, icoord/l
      write( 6,*) "INFO: Cyl jcoord    -->", jcoord, jcoord/m
!
      R = radius*uno
      R2a = (R-2)*(R-2)   ! lower radius
      R2b = (R+2)*(R+2)   ! upper radius
!
      do j = 1, m
         do i = 1, l
!
            d2 = (icoord-i)*(icoord-i)  &
                +(jcoord-j)*(jcoord-j) 
!        
            if((d2.gt.R2a).and.(d2.lt.R2b)) then
!
                obs(i,j) = 1
                nobs = nobs + 1
!
                imin = min(imin,i)
                jmin = min(jmin,j)
!
                imax = max(imax,i)
                jmax = max(jmax,j)
!
             endif
         end do
      end do
#endif
!
      write(6,*) "INFO: num. obs         -->", nobs
      write(6,*) "INFO: ratio obs/size   -->", nobs/float(l*m)
      write(6,*) "INFO: obs (x)             ",imax, imin
      write(6,*) "INFO: obs (y)             ",jmax, jmin
!
!#ifdef DEBUG_1
      if(myrank == 0) then
         write(6,*) "DEBUG1: Exiting from sub. build_obs"
      endif
!#endif
!
      end subroutine build_obs
