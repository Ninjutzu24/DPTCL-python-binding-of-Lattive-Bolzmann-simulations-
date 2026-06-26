!=====================================================================
!     ****** LBE/initialize
!
!     COPYRIGHT
!       (c) 2000-2011 by CASPUR/G.Amati
!       (c) 2013-20?? by CINECA/G.Amati
!     NAME
!       initialize
!     DESCRIPTION
!       Simple wrapper for different initializations routines..
!     INPUTS
!       itrestart
!       init_v
!       itfin
!       itstart
!       ivtim  
!       isignal
!       itsave
!     OUTPUT
!       none
!     TODO
!       
!     NOTES
!
!     *****
!=====================================================================
!
       subroutine initialize(itrestart,init_v,itfin,itstart,ivtim & 
                            ,isignal,itsave,icheck)
!
       use storage
       use timing
!
       implicit none
!
       integer             :: itstart
       integer, INTENT(in) :: itrestart,init_v, icheck
       integer, INTENT(in) :: itfin,ivtim,isignal,itsave
!
! some other initializations
!
       time_coll  = 0.0
       time_move  = 0.0
       time_obs   = 0.0
       time_bc    = 0.0
       time_mp    = 0.0
       time_dg    = 0.0
       time_io    = 0.0
       time_dev   = 0.0
       time_coll1 = 0.0
       time_move1 = 0.0
       time_obs1  = 0.0
       time_bc1   = 0.0
       time_mp1   = 0.0
       time_dg1   = 0.0
       time_io1   = 0.0
       time_dev1  = 0.0
       timeZ      = 0.0
       timeY      = 0.0
       timeX      = 0.0
       old1       = 0.0
       old2       = 0.0
       old3       = 0.0
       old4       = 0.0
       old5       = 0.0
       old6       = 0.0
!
! set boundary condition flags..
!
       call build_bcond
!
! build obstacles
!
#ifdef OBSTACLE
       call build_obs
       call vtk_obs
!       
! to fix       
       call w_obs               ! write ASCII file
!       call r_obs              ! read ASCII file
#endif
!
! build starting configuration
!
       if(itrestart.eq.1) then
!
          call time(tcountF0)
          call SYSTEM_CLOCK(countF0, count_rate, count_max)
!
          if(myrank == 0) then
             call restore_raw(itstart)
             write(*,*) "INFO: restoring at timestep ", itstart
          endif
!
          call SYSTEM_CLOCK(countF1, count_rate, count_max)
          call time(tcountF1)
          time_io = time_io + real(countF1-countF0)/(count_rate)
          time_io1 = time_io1 + tcountF1-tcountF0
!
#ifdef DEBUG_1
          if(myrank == 0) then
             write(6,*) "DEBUG1: I/O time (1)", real(countF1-countF0)/(count_rate)
             write(6,*) "DEBUG1: I/O time (2)", tcountF1-tcountF0
          endif
#endif
!
       else
          if(itrestart.ge.2) then
             itstart = 0
             write(6,*) "still not implemented"
             stop
          else
             itstart = 0
             call init(init_v)
!             call diagno(itstart)
             call varm(itstart)
             call prof_i(itstart,m/2)
             call prof_j(itstart,l/2)
#ifdef NO_BINARY
             call vtk_xy(itstart)
#else
             call vtk_xy_bin(itstart)
#endif
          endif
       endif
!
! compute collision parameters
       call hencol
!
       if(myrank==0) then
          call outdat(itfin,itstart,ivtim,isignal,itsave,icheck)
       endif
!
!
#ifdef NO_OUTPUT
! do nothing
#else
!       call prof_i(0,m/2)
!       call prof_j(0,l/2)
#endif
!
#ifdef DEBUG_1
       if(myrank == 0) then
          write(6,*) "DEBUG1: Exiting from sub. initialize"
       endif
#endif
!
# ifdef MEM_CHECK
       if(myrank == 0) then
          mem_stop = get_mem();
          write(6,*) "MEM_CHECK: after intialize. mem =", mem_stop
       endif
# endif
!
       end subroutine initialize
