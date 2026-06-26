! =====================================================================
!     ****** LBE/outdat
!
!     COPYRIGHT
!       (c) 2000-2011 by CASPUR/G.Amati
!       (c) 2013-20?? by CINECA/G.Amati
!     NAME
!       outdat
!     DESCRIPTION
!       write back simulation parameters
!       write in unit 16 (bgk.log)
!     INPUTS
!       itfin    --> end of the run
!       itstart  --> starting time
!       ivtim    --> interval between two different visualization
!       isignal  --> interval between two timings
!       itsave   --> time of saving
!       icheck   --> interval between two different diagnostic
!     OUTPUT
!       none
!     TODO
!       
!     NOTES
!       integer variables used: itfin,itstart,ivtim,isignal,itsave,icheck
!
!     *****
! =====================================================================
!
      subroutine outdat(itfin,itstart,ivtim,isignal,itsave,icheck)
!
      use storage
      implicit none
!
      integer, INTENT(in) :: itfin,itstart,ivtim
      integer, INTENT(in) :: isignal,itsave,icheck
!
      write(16,*) ' '
      write(16,*) '*********** size of the lattice **************'
      write(16,*) 'l (width x)  =',l
      write(16,*) 'm (width y)  =',m
      write(16,*) '*********** fluid data **********************'
      write(16,*) 'viscosity    =',svisc
      write(16,*) 'u0           =',u0
      write(16,*) 'u00          =',u00
      write(16,*) 'omega        =',omega1
      write(16,*) 'tau          =',1.0/omega1
      write(16,*) 'Reynolds     =',0.5*u0*l/svisc+0.5*u00*l/svisc
      write(16,*) 'forcing1     =',fgrad
      write(16,*) 'forcing2     =',u00/(6.0)
      write(16,*) 'u_inflow     =',u_inflow
      write(16,*) 'LES: cteS    =',cteS
      write(16,*) '*********** run data ************************'
      write(16,*) 'itfin        =',itfin
      write(16,*) 'itstart      =',itstart
      write(16,*) 'ivtim        =',ivtim
      write(16,*) 'isignal      =',isignal
      write(16,*) 'itsave       =',itsave
      write(16,*) 'icheck       =',icheck
      write(16,*) 'flag1        =',flag1
      write(16,*) 'flag2        =',flag2
      write(16,*) 'flag3        =',flag3
      write(16,*) 'ipad         =',ipad
      write(16,*) 'jpad         =',jpad
      write(16,*) 'radius       =',radius
      write(16,*) 'LES: cteS    =',cteS
      write(16,*) '************** Further check ****************'
      write(16,*) 'zero         =', zero, zero_qp
      write(16,*) 'uno          =', uno, uno_qp
      write(16,*) 'tre          =', tre, tre_qp
      write(16,*) 'rf           =', rf, rf_qp 
      write(16,*) 'qf           =', qf, qf_qp 
      write(16,*) 'p0           =', p0, p0_qp 
      write(16,*) 'p1           =', p1, p1_qp 
      write(16,*) 'p2           =', p2, p2_qp 
      write(16,*) '*********************************************'
      write(16,*) ' '

      write(6,*) '*********** size of the lattice **************'
      write(6,*) 'l (width x)  =',l
      write(6,*) 'm (width y)  =',m
      write(6,*) '****************fluid data*******************'
      write(6,*) 'viscosity    =',svisc
      write(6,*) 'u0           =',u0
      write(6,*) 'u00          =',u00
      write(6,*) 'omega        =',omega1
      write(6,*) 'tau          =',1.0/omega1
      write(6,*) 'Reynolds     =',0.5*u0*l/svisc+0.5*u00*l/svisc
      write(6,*) 'forcing1     =',fgrad
      write(6,*) 'forcing2     =',u00/(6.0)
      write(6,*) 'u_inflow     =',u_inflow
      write(6,*) 'LES: cteS    =',cteS
      write(6,*) '**************** run data********************'
      write(6,*) 'itfin        =',itfin
      write(6,*) 'itstart      =',itstart
      write(6,*) 'ivtim        =',ivtim
      write(6,*) 'isignal      =',isignal
      write(6,*) 'itsave       =',itsave
      write(6,*) 'icheck       =',icheck
      write(6,*) 'flag1        =',flag1
      write(6,*) 'flag2        =',flag2
      write(6,*) 'flag3        =',flag3
      write(6,*) 'ipad         =',ipad
      write(6,*) 'jpad         =',jpad
      write(6,*) 'radius       =',radius
      write(6,*) '*********************************************'
      write(6,*) ' '

#ifdef DEBUG_1
      if(myrank == 0) then
         write(6,*) "DEBUG1: Exiting from sub. outdat"
      endif
#endif
!
      end subroutine outdat
