!=====================================================================
!     ****** LBE/finalize
!
!     COPYRIGHT
!       (c) 2000-2011 by CASPUR/G.Amati
!       (c) 2013-20?? by CINECA/G.Amati
!     NAME
!       bcond
!     DESCRIPTION
!       Simple wrapper for different finalizations..
!     INPUTS
!       itstart
!       itstart      
!     OUTPUT
!       none
!     TODO
!       
!     NOTES
!
!     *****
!=====================================================================
!
      subroutine finalize(itstart,itfin)
!
      use storage
      use timing
!
      implicit none
!
      integer, INTENT(in) :: itstart, itfin
!
! write on external file some performance figure...
!
      if(myrank == 0) then 
         open(69,file='bgk.perf',  status='unknown')
         write(69,9999) 
         write(69,*)  "# Run info "
         write(69,*)  l, m
         write(69,9999) 
         write(69,*)  "# Time for section "
         write(69,1100) time_init, time_init1
         write(69,1101) time_loop, time_loop1
         write(69,1102) time_coll, time_coll1
         write(69,1103) time_move, time_move1
         write(69,1104) time_bc, time_bc1
         write(69,1105) time_io, time_io1
         write(69,1114) time_dg, time_dg1
         write(69,1116) time_obs, time_obs1
         write(69,1117) time_loop-(time_coll+time_move+time_bc+time_io+time_dg+time_mp+time_obs)
         write(69,9999)
         write(69,*)  "# Ratio per section (loop)"
         write(69,1202) time_io/time_loop  , time_io1/time_loop1
         write(69,1203) time_bc/time_loop  , time_bc1/time_loop1
         write(69,1204) time_coll/time_loop, time_coll1/time_loop1
         write(69,1205) time_dg/time_loop  , time_dg1/time_loop1
         write(69,1206) time_move/time_loop, time_move1/time_loop1
         write(69,1207) time_obs/time_loop , time_obs1/time_loop1
         write(69,9999) 
         write(69,*)  "# Derived (global) metrics "
         write(69,1106) float(l)*float(m)* & 
                        (itfin-itstart)/(time_loop1)/1000.0/1000.0
         write(69,1107) float(130)*float(l)*float(m)*  &
                        (itfin-itstart)/(time_coll)/1000.0/1000.0
         write(69,1108) float(9*8)*float(l)*float(m)* &
                        (itfin-itstart)/(time_coll)/1000.0/1000.0
         write(69,1109) float(8*8)*float(l)*float(m)* &
                        (itfin-itstart)/(time_move)/1000.0/1000.0
         write(69,9999) 
         write(69,*)  "# Memory (task 0) metrics "
         write(69,1110) mem_start, mem_stop
         write(69,*) (l/1024.0)*(m/1024.0)*9*2*4
         write(69,9999) 
         close(69)   ! bgk.perf
!
         write(6,1106) float(l)*float(m)* &
                       (itfin-itstart)/(time_loop1)/1000.0/1000.0

!
!         call system("git log | grep commit | tail -n 1 >> bgk.perf")
!
      endif
!
!     closing files...
!
      close(16)   ! bgk.log
      close(31)   ! restore.bin
      close(60)   ! prof_k.dat
      close(61)   ! prof_i.dat
      close(64)   ! prof_j.dat
      close(62)   ! u_med.dat
      close(68)   ! probe.dat
      close(63)   ! diagno.dat
!
! formats
!
9999  format(" #--------------------------------")
1100  format(" # init   time",2(e14.6,1x))
1101  format(" # loop   time",2(e14.6,1x))
1102  format(" # coll   time",2(e14.6,1x))
1103  format(" # move   time",2(e14.6,1x))
1104  format(" # bc     time",2(e14.6,1x))
1105  format(" # I/O    time",2(e14.6,1x))
1114  format(" # diagno time",2(e14.6,1x))
1116  format(" # Obst   time",2(e14.6,1x))
1117  format(" # Check      ",1(e14.6,1x))
1202  format(" # Ratio I/O  ",2(f7.3,1x))
1203  format(" # Ratio BC   ",2(f7.3,1x))
1204  format(" # Ratio Coll ",2(f7.3,1x))
1205  format(" # Ratio Diag.",2(f7.3,1x))
1206  format(" # Ratio Move ",2(f7.3,1x))
1207  format(" # Ratio Obs  ",2(f7.3,1x))
1106  format(" # Mlups      ",1(f14.6,1x))
1107  format(" # coll   Flop",1(e14.6,1x), "MFlops")
1108  format(" # coll   BW  ",1(e14.6,1x), "GB/s")    ! double precision only
1109  format(" # move   BW  ",1(e14.6,1x), "GB/s")    ! double precision only
1110  format(" # Memory (start,stop)",2(f14.6,1x), "MB")  ! double precision only
!
#ifdef DEBUG_1
      if(myrank == 0) then
         write(6,*) "DEBUG1: Exiting from sub. finalize"
      endif
#endif
!
# ifdef MEM_CHECK
      if(myrank == 0) then
         mem_stop = get_mem();
         write(6,*) "MEM_CHECK: after sub. finalize mem =", mem_stop
      endif
# endif

        return
        end subroutine finalize
