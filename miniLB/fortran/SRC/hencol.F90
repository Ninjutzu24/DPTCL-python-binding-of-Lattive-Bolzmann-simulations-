! =====================================================================
!     ****** LBE/hencol
!
!     COPYRIGHT
!       (c) 2000-2011 by CASPUR/G.Amati
!       (c) 2013-20?? by CINECA/G.Amati
!     NAME
!       hencol
!     DESCRIPTION
!       computes collision parameters and the forcing term
!       according to the following equations:
!       omega = 2.0/(6.0*svisc+1.0)
!       fgrad = 4.0*u0*svisc/(3.0*float(n)*float(n))
!     INPUTS
!       none
!     OUTPUT
!       none
!     TODO
!
!     NOTES
!
!     *****
! =====================================================================
!
        subroutine hencol
!
        use storage
        implicit none
!
        omega  = (uno+uno)/((6.0*svisc)+1.0)
        omega1 = (uno+uno)/((6.0*svisc)+1.0)
!
#ifdef HALF_P
# ifdef MIXEDPRECISION
! issues with namelist and half precision
        write(6,*)  "WARNING: Hand-made forcing..."
        write(16,*) "WARNING: Hand-made forcing..."
        u0 = 0.0
!        svisc = 0.05
!        omega = 1.53846153846153846153
        write(6,*)  "WARNING: ", omega, u0, svisc
        write(16,*) "WARNING: ", omega, u0, svisc
# else
! nothing to do        
# endif
#endif
!
! forcing term
!
        fgrad = 4.0*u0*svisc/(3.0*real(m,mykind)*real(m,mykind))
!
        if(myrank == 0) then
           if(fgrad.le.0.0000001) then
              write( 6,*) "WARNING: volume forcing below 10e-7" 
              write(16,*) "WARNING: volume forcing below 10e-7" 
           endif
        endif
!
#ifdef DEBUG_1
       if(myrank == 0) then
          write(6,*) "DEBUG1: omega, fgrad",omega,fgrad
          write(6,*) "DEBUG1: Exiting from sub. hencol"
       endif
#endif
!
        end subroutine hencol
