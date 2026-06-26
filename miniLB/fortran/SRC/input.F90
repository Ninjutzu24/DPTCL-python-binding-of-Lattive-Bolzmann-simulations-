! =====================================================================
!     ****** LBE/input
!
!     COPYRIGHT
!       (c) 2000-2008 by CASPUR/G.Amati
!       (c) 2013-20?? by CINECA/G.Amati
!     NAME
!       input
!     DESCRIPTION
!       read input parameters
!       read from unit 15 (bgk.input)
!     INPUTS
!       none
!     OUTPUT
!       size     --> size_x, size_y
!       itfin    --> end of the run
!       ivtim    --> interval between two different visualization
!       isignal  --> interval between two timings 
!       itsave   --> time of saving
!       icheck   --> interval between two different diagnostic
!       irestart --> restart from file (1=yes,other=no)
!       init_v   --> initial velocity condition (see subroutine init)
!       radius   --> radius for flow around cylinder
!     TODO
!	
!     NOTES
!       integer variables used: itfin,ivtim,isignal,itsave,icheck,irestart
!                               init_v
!
!     *****
! =====================================================================
!
      subroutine input (itfin,ivtim,isignal,itsave,icheck,irestart, & 
                          init_v)
!
      use storage
      use timing
! 
      implicit none
!
      integer:: itfin,ivtim,isignal,itsave,icheck
      integer:: irestart,init_v
!
      namelist /parameters/ svisc, u0, itfin, ivtim, isignal, & 
                            itsave, icheck, irestart, init_v, &
                            lx, ly,                           &
                            flag1, flag2, flag3, ipad, jpad,  & 
                            radius,cteS
!
! default values
      flag1 = 0         ! creating obstacles  (1-file/2-creating)
      flag2 = 0         ! obstacles           (1-sphere/2-cilinder)
      flag3 = 0 
      init_v= 0
      cteS  = 0.1       ! smagorinsky constant
!
      ipad  = 0         ! no memory padding (x)
      jpad  = 0         ! no memory padding (y)
!
! Radius for flow around a cylinder      
      radius = 16
!      
      open(15,FILE='bgk.input',STATUS='old')
      read(15,parameters)
      close(15)

      l = lx
      m = ly
!
! some fix..
      l1 = l+1
      m1 = m+1
!
! a check
#ifdef OBSTACLE
      if (radius.gt.m/8) then
         write(6,*) "WARNING: radius size =", radius, m
      endif      
!
      if (radius.gt.(m-2)) then
         write(6,*) "ERROR: radius too big =", radius, m
         stop
      endif      
#endif      
!
! default value: volume forcing along x
!
      u0x = zero
      u0y = zero
!
#ifdef DEBUG_1
      if(myrank == 0) then
         write(6,*) "DEBUG1: Exiting from sub. input"
      endif
#endif
!
# ifdef MEM_CHECK
      if(myrank == 0) then
         mem_stop = get_mem();
         write(6,*) "MEM_CHECK: after sub. input mem =", mem_stop
      endif
# endif
!
      end subroutine input
