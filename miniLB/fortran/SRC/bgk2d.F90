! =====================================================================
!     ****** LBE/bgk2D
!
!     COPYRIGHT
!       (c) 2021 by CINECA/G.Amati
!     NAME
!       bgk2d
!     DESCRIPTION
!       main program for LBM 2D
!     INPUTS
!       none
!     OUTPUT
!       none
!     TODO
!
!     NOTES
!
!       integer variables used: itfin, itstart, ivtim
!                               itime, itsave, icheck, itrestart
!                               isignal
!       real variables used: tempo1, tempo2     
!       open the following unit: 16 (bgk.log)
!                                60 (prof_k.dat)
!                                61 (prof_i.dat)
!                                62 (u_med.dat)
!                                63 (diagno.dat)
!                                68 (probe.dat)
!                                69 (bgk.perf)
!       velocity directions:
!        direction  1    unit vector = ( 1,-1)   kind 2 (diagonal)
!        direction  3    unit vector = ( 1, 1)   kind 2 (diagonal)
!        direction  5    unit vector = ( 1, 0)   kind 1 (horizontal)
!        direction  8    unit vector = ( 0, 1)   kind 1 (vertical)
!        direction 10    unit vector = (-1,-1)   kind 2 (diagonal)
!        direction 12    unit vector = (-1, 1)   kind 2 (diagonal)
!        direction 14    unit vector = (-1, 0)   kind 1 (horizontal)
!        direction 17    unit vector = ( 0,-1)   kind 1 (vertical)
!        direction 19    unit vector = ( 0, 0)   kind 0 (rest population)
!                              
!     *****
! =====================================================================
!
      program bgk2d
!
      use storage
      use timing
      use real_kinds
!
      implicit none
!
      INTEGER:: itfin, itstart, ivtim
      INTEGER:: itime, itsave, icheck, itrestart, init_v
      INTEGER:: isignal
!
      call SYSTEM_CLOCK(countH0, count_rate, count_max)
      call time(tcountH0)
!      
! set up the simulation...
      call setup(itfin,ivtim,isignal,itsave,icheck,itrestart, & 
                 init_v)
!      
! initialize the flow...
      call initialize(itrestart,init_v,itfin,itstart,ivtim,isignal, & 
                      itsave,icheck)
!
#ifdef NOMANAGED
!$acc data copyin(a01,a03,a05,a08,a10,a12,a14,a17,a19,b01,b03,b05,b08,b10,b12,b14,b17,b19,obs)
#endif
!
      call SYSTEM_CLOCK(countH1, count_rate, count_max)
      call time(tcountH1)
      time_init =  real(countH1-countH0)/(count_rate)
      time_init1 = (tcountH1-tcountH0)
!      
      call SYSTEM_CLOCK(countE0, count_rate, count_max)
      call time(tcountE0)
!
      call SYSTEM_CLOCK(countD0, count_rate, count_max)
      call time(tcountD0)
!
! main loop starts here.....
!
      if(myrank==0) then 
         call system("date       >> time.log")
      endif
!
#ifdef OFFLOAD
!$OMP target data map(tofrom:  a01,a03,a05,a08,a10,a12,a14,a17,a19, & 
!$OMP&                         b01,b03,b05,b08,b10,b12,b14,b17,obs) 
#endif
!      
      do itime=itstart+1,itfin
!
#ifdef DEBUG_2
         if(myrank == 0) then
            write(6,*) "DEBUG2: starting time step =", itime
         endif
#endif
!
         call boundaries         ! boundary conditions
         call propagation        ! propagation step
         call collision(itime)   ! collision step
!
! get macroscopic values
         call diagnostic(itime,ivtim,icheck,itsave)
!
! get timing/profiling values
         if (mod(itime,isignal).eq.0) then
            if (myrank == 0 ) then 
               call profile(itime,itfin,isignal) 
            endif
         endif
      enddo
!
!     some global timings
      call SYSTEM_CLOCK(countE1, count_rate, count_max)
      call time(tcountE1)
      time_loop = real(countE1-countE0)/(count_rate)
      time_loop1 = tcountE1-tcountE0
!
!     final diagnostic (for check)
      call diagno(itime-1)
      call varm(itime-1)
      call prof_i(itime-1,m/2)
      call prof_j(itime-1,l/2)
!
#ifdef OFFLOAD
!$omp end target data
#endif
!      
#ifdef NOMANAGED
!$acc end data
#endif
!
      call finalize(itstart,itfin)     ! finalize all
!
      if(myrank==0) then
         call system("date       >> time.log")
         write(6,*) "That's all folks!!!!"
      endif
!
      end program bgk2d
