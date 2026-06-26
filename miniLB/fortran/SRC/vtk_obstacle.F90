!=====================================================================
!     ****** LBE/vtk_obs
!
!     COPYRIGHT
!       (c) 2000-2011 by CASPUR/G.Amati
!       (c) 2013-20?? by CINECA/G.Amati
!     NAME
!       vtk_obs
!     DESCRIPTION
!       visualization routine: vtk file with obstacle (ASCII file)
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
      subroutine vtk_obs
!
      use timing
      use storage
!
      implicit none
!
      integer :: i, j
      integer :: itime
      character(len=24) :: file_nameVTK
!
      file_nameVTK  = 'tec_ob.xxxxxxxx.vtk'
!
      myrank = 0
      itime  = 0
!
! dumping obstacle 
!
! hdf5/binary/vtk dump section
!
#ifdef NO_OUTPUT
      if(myrank==0) then 
         write(6,*)  "INFO: no output mode enabled, no dump at all"
         write(16,*) "INFO: no output mode enabled, no dump at all"
      endif
#endif
!
      write(6,*) "task", myrank, "has ", nobs, " obstacles"
! 
      write(file_nameVTK(8:15),4000) itime
      write(6,*) "INFO: obstacle vtk dump ", file_nameVTK
!
      open(52,file=file_nameVTK,status='unknown')
      write(52,'(A26)')'# vtk DataFile Version 2.0'
      write(52,'(A5)')'Campo'
      write(52,'(A5)')'ASCII'
      write(52,'(A24)')'DATASET RECTILINEAR_GRID'
      write(52,'(A11,I10,A1,I10,A3)')  & 
               'DIMENSIONS ',l,' ',m,' 1'
!
      write(52,'(A14,I10,A7)')'X_COORDINATES ',l,' double'
      do i = 1,l
         write(52, *) i 
      enddo
!
      write(52,'(A14,I10,A7)')'Y_COORDINATES ',m,' double'
      do j = 1,m
         write(52, *) j 
      enddo
!
      write(52,'(A14,I10,A7)')'Z_COORDINATES ',1,' double'
      write(52, *) 1
!               
      write(52,'(A10,I10)')'POINT_DATA ',l*m
      write(52,'(A21)')'SCALARS obs float'
      write(52,'(A20)')'LOOKUP_TABLE default'
      do j = 1,m
         do i = 1,l
            write(52,*) obs(i,j)*1.0
         end do
      end do
      write(6,*) "INFO: obs vtk dump done"
      close(52) ! vtk obs. dump file
!
#ifdef DEBUG_1
      if(myrank == 0) then
         write(6,*) "DEBUG1: Exiting from sub. vtk_obs"
      endif
#endif
!
4000    format(i8.8)
!
      end subroutine vtk_obs
