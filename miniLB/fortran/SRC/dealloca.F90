! =====================================================================
!     ****** LBE/dealloca
!
!     COPYRIGHT
!       (c) 2000-2011 by CASPUR/G.Amati
!       (c) 2013-20?? by CINECA/G.Amati
!     NAME
!       dealloca
!     DESCRIPTION
!       main program for LBM 3D
!     INPUTS
!       none
!     OUTPUT
!       none
!     TODO
!
!     NOTES
! =====================================================================

subroutine dealloca()
      use storage; 
      implicit none
!
      deallocate(a01)
      deallocate(a03)
      deallocate(a05)
      deallocate(a08)
      deallocate(a10)
      deallocate(a12)   
      deallocate(a14)
      deallocate(a17)
      deallocate(a19)
!
      deallocate(b01)
      deallocate(b03)
      deallocate(b05)
      deallocate(b08)
      deallocate(b10)
      deallocate(b12)   
      deallocate(b14)
      deallocate(b17)
      deallocate(b19)
!
#ifdef DEBUG_1
        if(myrank == 0) then
           write(6,*) "DEBUG1: Exiting from sub. dealloca"
        endif
#endif
!
end subroutine dealloca

