!=====================================================================
!     ****** LBE/r_obs
!
!     COPYRIGHT
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
!
!     *****
!=====================================================================
!
      subroutine r_obs
!
      use timing
      use storage
!
      implicit none
!
      integer:: i, j
      integer:: ii, jj                   ! dumb variables 
      real(mykind):: d2, R2a, R2b, R
      real(mykind):: hsize
!
! write obstacle.dat
!
      open(98,file="obstacle.dat", status='unknown')
!
      nobs = 0                  ! reset counter
!      
      do j = 1, m
         do i = 1, l
!
            read(98,*) ii,jj,obs(i,j)
!
            if(obs(i,j).EQ.1) then
               nobs = nobs + 1
!
               imin = min(imin,i)
               jmin = min(jmin,j)
!
               imax = max(imax,i)
               jmax = max(jmax,j)
!
            endif
!
         end do
      end do
      close(98)
!
      write(6,*) "INFO: num. obs         -->", nobs
      write(6,*) "INFO: ratio obs/size   -->", nobs/float(l*m)
      write(6,*) "INFO: obs (x)             ", imax, imin
      write(6,*) "INFO: obs (y)             ", jmax, jmin
!
!#ifdef DEBUG_1
      if(myrank == 0) then
         write(6,*) "DEBUG1: Exiting from sub. r_obs"
      endif
!#endif
!
      end subroutine r_obs
