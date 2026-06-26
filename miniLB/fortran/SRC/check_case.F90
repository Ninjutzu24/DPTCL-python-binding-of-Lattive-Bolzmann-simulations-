!=====================================================================
!     ****** LBE/check_case
!
!     COPYRIGHT
!       (c) 2000-2011 by CASPUR/G.Amati
!       (c) 2013-20?? by CINECA/G.Amati
!     NAME
!       bcond
!     DESCRIPTION
!       check of the simulation and of the different 
!       pre-proc flags activated
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
        subroutine check_case
!
        use storage
        use timing
!
        implicit none
!
! Warning section  (run will proceed...) 
        if((u0.GT.0).AND.(u_inflow.GT.0)) then
           write(16,*) "WARNING: inflow ad volume force & 
              &         at the same time", myrank, u0, u_inflow 
        endif
!
! Info section   
        call git_info
!        
! boundary conditions
#ifdef PERIODIC
        write(6 ,*) "INFO: The test case has periodic bc (-DPERIODIC)" 
        write(16,*) "INFO: The test case has periodic bc (-DPERIODIC)" 
#elif CHANNEL
        write(6 ,*) "INFO: The test case has channel bc (-DCHANNEL)" 
        write(16,*) "INFO: The test case has channel bc (-DCHANNEL)" 
#elif INFLOW
        write(6 ,*) "INFO: The test case has inflow bc (-DINFLOW)" 
        write(16,*) "INFO: The test case has inflow bc (-DINFLOW)" 
#elif MYVERSION
        write(6 ,*) "INFO: The test case has ad-hoc bc (-DMYVERSION)" 
        write(16,*) "INFO: The test case has ad-hoc bc (-DMYVERSION)" 
#else
        write(6 ,*) "INFO: The test case is driven cavity (default)" 
        write(16,*) "INFO: The test case is driven cavity (default)" 
#endif
!
#ifdef OBSTACLE
        write(6 ,*)  "INFO: The test case has an obstacle in the flow"
        write(16,*)  "INFO: The test case has an obstacle in the flow"
#endif
!
#ifdef NOSHIFT
        write(6,*)  "INFO: using NOSHIFT preprocessing flag"
        write(16,*) "INFO: using NOSHIFT preprocessing flag"
#endif
!
#ifdef DRAG
        write(6,*)  "INFO: using DRAG preprocessing flag"
        write(16,*) "INFO: using DRAG preprocessing flag"
        write(6,*)  "WARNING: the  box for drag/lift is hard-coded, & 
                     please check!"
        write(16,*) "WARNING: the  box for drag/lift is hard-coded, & 
                     please check!"
#endif
!
#ifdef QUAD_P
        write(16,*) "INFO: using quad precision (storage)"
#elif DOUBLE_P
        write(16,*) "INFO: using double precision (storage)"
#elif HALF_P
        write(16,*) "INFO: using half precision (storage)"
        write(6,*)  "WARNING: pure half precision has some problem"
#else
        write(16,*) "INFO: using single precision (storage)"
#endif
!
#ifdef MIXEDPRECISION
       write(16,*) "INFO: using mixed precision"
#else
       write(16,*) "INFO: using the same precision for computation"
#endif
!
       write(16,*) "INFO: mykind=   ", mykind,    "range  =", range(u0)
       write(16,*) "INFO: mykind=   ", mykind,    "huge   =", huge(u0)
       write(16,*) "INFO: mykind=   ", mykind,    "epsilon=", epsilon(u0)
       write(16,*) "INFO: mystorage=", mystorage, "range  =", & 
                                       range(a01(1,1))
       write(16,*) "INFO: mystorage=", mystorage, "huge   =", &
                                       huge(a01(1,1))
       write(16,*) "INFO: mystorage=", mystorage, "epsilon=", &
                                       epsilon(a01(1,1))
!
       write(16,*) "INFO: using RAW I/O"
!
#ifdef NO_BINARY
       write(6,*)  "INFO: vtk output in ASCII (debug mode)"
       write(16,*) "INFO: vtk output in ASCII (debug mode)"
#endif
!
#ifdef LES
       write(6,*) "WARNING: LES (Smagorinsky) enabled UNDER DEVELOPMENT"
       write(16,*)"WARNING: LES (Smagorinsky) enabled UNDER DEVELOPMENT"
#endif
!
#ifdef PGI
       write(16,*) "      quad precision not supported"
#endif
!
#ifdef DOCONCURRENT
       write(6,*)  "INFO: do concurrent version (GPU)"
       write(16,*) "INFO: do concurrent version (GPU)"
#elif MULTICORE
       write(6,*)  "INFO: multicore parallelization (CPU)"
       write(16,*) "INFO: multicore parallelization (CPU)"
#elif OFFLOAD
       write(6,*)  "INFO: offload version (GPU)"
       write(16,*) "INFO: offload version (GPU)"
#elif OPENACC
       write(6,*)  "INFO: openacc version (GPU)"
       write(16,*) "INFO: openacc version (GPU)"
#else
       write(6,*)  "INFO: serial version (CPU)"
       write(16,*) "INFO: serial version (CPU)"
#endif
!
#ifdef FUSED
       write(16,*) "INFO: using Fused version"
#else
       write(16,*) "INFO: using Move & collide (Original) version"
#endif
!
#ifdef DEBUG_3
       if(myrank == 0) then
          write(6,*) "INFO: DEBUG3 mode enabled"
       endif
#endif
!
#ifdef DEBUG_2
       if(myrank == 0) then
          write(6,*) "INFO: DEBUG2 mode enabled"
       endif
#endif
!
#ifdef DEBUG_1
       if(myrank == 0) then
       endif
#endif
!
#ifdef TRICK1
       write(6,*) "WARNING: square box mandatory (TRICK1)!"
       write(16,*) "WARNING: square box mandatory (TRICK1)!"
       if(l.ne.m) then
          write(6,*) "ERROR: box not squared (TRICK1)!"
          write(16,*) "ERROR: box not squared (TRICK1)!"
          stop
       endif
#endif
!
#ifdef TRICK2
       write(6,*) "WARNING: forced offload num_threads(TRICK2)!"
       write(16,*) "WARNING: forced offload num_threads(TRICK2)!"
#endif
!
#ifdef DEBUG_1
       if(myrank == 0) then
          write(6,*) "INFO: DEBUG1 mode enabled"
          write(6,*) "DEBUG1: Exiting from sub. check_case"
       endif
#endif
        end subroutine check_case
