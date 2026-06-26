!====================================================================
!     ****** LBE/setup
!
!     COPYRIGHT
!       (c) 2000-2011 by CASPUR/G.Amati
!       (c) 2013-20?? by CINECA/G.Amati
!     NAME
!       setup
!     DESCRIPTION
!       Simple wrapper for different initializations routines..
!     INPUTS
!       
!     OUTPUT
!       
!     TODO
!       
!     NOTES
!
!     *****
!=====================================================================
!
      subroutine setup(itfin,ivtim,isignal,itsave,icheck,itrestart, & 
                       init_v)
!
      use storage
      use timing
! 
      implicit none
!
      INTEGER:: itfin, ivtim
      INTEGER:: itsave, icheck, init_v
      INTEGER:: isignal
      INTEGER:: itrestart
      character(len=19) :: file_name2
      character(len=19) :: file_name3
      character(len=9)  :: file_name4
      character(len=15) :: file_name5
      character(len=19) :: file_name6
      character(len=19) :: file_name7
      character(len=21) :: file_name8   ! draglift
      character(len=21) :: file_name9   ! bgktime.log
!
! set values for serial version...
      myrank = 0
!
      call system("date       > time.log")
!
! prof_i
      file_name2 = 'prof_i.dat'
!
! probe
      file_name3 = 'probe.dat'
!
! task
      file_name5 = 'task.log'
!
! prof_j
      file_name6 = 'prof_j.dat'
!
! prof_j
      file_name7 = 'probe_visc.dat'
!
! draglift
      file_name8 = 'drag.lift.dat'
!
! time
      file_name9 = 'bgk.time.log'
!
      open(16,file='bgk.log',  status='unknown')
      open(61,file=file_name2, status='unknown')        ! prof_i
      open(68,file=file_name3, status='unknown')        ! probe
      open(38,file=file_name5, status='unknown')        ! task.XXXXXX.log
      open(64,file=file_name6, status='unknown')        ! prof_j
      open(65,file=file_name7, status='unknown')        ! probe_visc
      open(66,file=file_name8, status='unknown')        ! drag
      open(99,file=file_name9, status='unknown')        ! bgk.time.log
!
      open(63,file='diagno.dat',status='unknown')       ! diagno.dat
!
      if(myrank==0) then 
         write(6,*) "================================"
         write(6,*) "        Uniform LBE...          "
! 
! implementation
#ifdef FUSED
         write(6,*) " Implementation: FUSED   "
#else
         write(6,*) " Implementation: ORIGINAL"
#endif
! 
! parallelization
#ifdef DOCONCURRENT
         write(6,*) " GPU Parallization: DO concurrent   "
#elif MULTICORE
         write(6,*) " CPU Parallelization: multicore"
#elif OFFLOAD
         write(6,*) " GPU Parallelization: OpenMP offload"
#elif OPENACC 
         write(6,*) " GPU Parallelization: OpenACC"
#else  
         write(6,*) " Serial"
#endif
! 
! test case
#ifdef TGV
         write(6,*) " Test Case: Taylor-Green Vortices"
#elif POF  
         write(6,*) " Test Case: Poiseuille Flow"
#elif VKS  
         write(6,*) " Test Case: Von Karman Street"
#else         
         write(6,*) " Test Case: Lid-Driven Cavity"
#endif
!
         write(6,*) "================================"
      endif
!
! check, read input data and data allocation
!
      call check_case
!
      call input(itfin,ivtim,isignal,itsave,icheck, & 
                   itrestart,init_v)
!
      file_name4 = 'u_med.dat'
      open(62,file=file_name4, status='unknown')
!
      call alloca()
!
#ifdef DEBUG_1
      if(myrank == 0) then
         write(6,*) "DEBUG1: Exiting from sub. setup"
      endif
#endif
!
# ifdef MEM_CHECK
      if(myrank == 0) then
         mem_stop = get_mem();
         write(6,*) "MEM_CHECK: after sub. setup mem =", mem_stop
      endif
# endif
!
      end subroutine setup
