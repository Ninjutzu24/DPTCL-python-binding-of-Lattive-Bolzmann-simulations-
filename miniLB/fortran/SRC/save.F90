! =====================================================================
!     ****** LBE/save
!
!     COPYRIGHT
!       (c) 2000-2011 by CASPUR/G.Amati
!       (c) 2013-20?? by CINECA/G.Amati
!     NAME
!       save
!     DESCRIPTION
!       Simple wrapper for saving in raw
!     INPUTS
!       itime --> timestep
!     OUTPUT
!
!     TODO
!	
!     NOTES
!
!     *****
! =====================================================================
!
      subroutine save (itime)
!
      use storage
      use timing
      use real_kinds
!
      implicit none
!
      integer, INTENT(in) :: itime
!
      call system("date")
      call time(tcountF0)
      call SYSTEM_CLOCK(countF0, count_rate, count_max)
!
      call save_raw(itime)
!
      call SYSTEM_CLOCK(countF1, count_rate, count_max)
      call time(tcountF1)
      time_io = time_io + real(countF1-countF0)/(count_rate)
      time_io1 = time_io1 + tcountF1-tcountF0
!
# ifdef MEM_CHECK
      if(myrank == 0) then
         mem_stop = get_mem();
         write(6,*) "MEM_CHECK: after sub. save mem =", mem_stop
      endif
# endif
!
      end subroutine save
