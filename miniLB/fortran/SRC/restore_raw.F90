!=======================================================================
!     ****** LBE/restore_raw
!
!     COPYRIGHT
!       (c) 2000-2011 by CASPUR/G.Amati
!       (c) 2013-20?? by CINECA/G.Amati
!     NAME
!       restore_raw
!     DESCRIPTION
!       restore all microscopic variables (all populations)
!       read from unit 30 (restore.bin, unformatted)
!     INPUTS
!       none
!     OUTPUT
!       itime --> timestep
!     TODO
!      
!     NOTES
!       integer itime,i,j
!
!     *****
!=======================================================================
!
      subroutine restore_raw(itime)
!
      use storage
      use timing
      implicit none
!
      character(len=14) :: file_name
!
      integer, INTENT(out) :: itime
      integer              :: i,j
!
      file_name = 'restore.bin'
      open(30,file=file_name,form="unformatted",status='unknown')
!
      write(16,*) 'INFO: I am restoring from ', file_name
      write(6,*)  'INFO: I am restoring from ', file_name
!
      read(30) itime
!
      read(30) ((a01(i,j),i=0,l1),j=0,m1)
      read(30) ((a03(i,j),i=0,l1),j=0,m1)
      read(30) ((a05(i,j),i=0,l1),j=0,m1)
      read(30) ((a08(i,j),i=0,l1),j=0,m1)
      read(30) ((a10(i,j),i=0,l1),j=0,m1)
      read(30) ((a12(i,j),i=0,l1),j=0,m1)
      read(30) ((a14(i,j),i=0,l1),j=0,m1)
      read(30) ((a17(i,j),i=0,l1),j=0,m1)
      read(30) ((a19(i,j),i=0,l1),j=0,m1)
!
      close(30)
!
#ifdef DEBUG_1
      if(myrank == 0) then
         write(6,*) "DEBUG1: Exiting from sub. restore_raw"
      endif
#endif
!
# ifdef MEM_CHECK
      if(myrank == 0) then
         mem_stop = get_mem();
         write(6,*) "MEM_CHECK: after sub. restore_raw mem =", mem_stop
      endif
# endif
!
      end subroutine restore_raw
