!=====================================================================
!     ****** LBE/w_obs
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
      subroutine w_obs
!
      use timing
      use storage
!
      implicit none
!
      integer:: i, j
      real(mykind):: d2, R2a, R2b, R
      real(mykind):: hsize
!
! write obstacle.dat
!
      open(98,file="obstacle.dat", status='unknown')
!
      do j = 1, m
         do i = 1, l
!
            write(98,*) i,j,obs(i,j)
!
         end do
      end do
      close(98)
!
#ifdef DEBUG_1
      if(myrank == 0) then
         write(6,*) "DEBUG1: Exiting from sub. w_obs"
      endif
#endif
!
      end subroutine w_obs
