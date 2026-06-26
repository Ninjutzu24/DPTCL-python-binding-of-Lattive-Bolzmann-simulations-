!=======================================================================
!     ****** LBE/vtk_om
!
!     COPYRIGHT
!       (c) 2009 by CASPUR/G.Amati
!       (c) 2000-2011 by CASPUR/G.Amati
!       (c) 2013-20?? by CINECA/G.Amati
!     NAME
!       vtk_om
!     DESCRIPTION
!       Graphic subroutine:
!       write ASCII output for VTK with vorticity + stream field
!       write on unit 55 (tec_om.xxxxxxx.dat) where 
!                                     xxxxxxx is the timestep 
!       the file is closed at the end of the subroutine
!     INPUTS
!       itime   ---> timestep
!     OUTPUT
!       none
!     TODO
!       
!     NOTES
!       character used:  file_name (19)
!       integer variables used: itime,i,k
!       max time allowed  99'999'999
!       single precision only, to saving space..
!
!     BUGS
!
!     *****
!=======================================================================
!
        subroutine vtk_om(itime)
!
        use storage
        implicit none
!
        integer ::  i,j,itime
!	
        character(len=24) :: file_name
!
        real(sp) ::  up1, down1, left1, right1
        real(sp) ::  d_up, d_down, d_left, d_right
        real(sp) ::  vorticity
!
        file_name = 'tec_om.xxxxxxxx.vtk'
!
        myrank = 0
!
        write(file_name(8:15),4000) itime
        open(55,file=file_name,status='unknown')
!
        write(55,'(A26)')'# vtk DataFile Version 2.0'
        write(55,'(A5)')'Campo'
        write(55,'(A5)')'ASCII'
        write(55,'(A24)')'DATASET RECTILINEAR_GRID'
        write(55,'(A11,I10,A1,I10,A1,I10)') &
                              'DIMENSIONS ',l-2,' ',m-2,' ',1
!
        write(55,'(A14,I10,A7)')'X_COORDINATES ',l-2,' double'
        do i = 2,l-1
           write(55, *) i + offset(1)
        enddo
!
        write(55,'(A14,I10,A7)')'Y_COORDINATES ',m-2,' double'
        do j = 2,m-1
           write(55, *) j + offset(2)
        enddo
!
        write(55,'(A14,I10,A7)')'Z_COORDINATES ',1,' double'
        write(55, *) 0
!
        write(55,'(A10,I10)')'POINT_DATA ',(l-2)*(m-2)
        write(55,'(A25)')'SCALARS vorticity double'
        write(55,'(A20)')'LOOKUP_TABLE default'
        do j = 2,m-1
           do i = 2,l-1
!
           d_up= a01(i,j+1)+a03(i,j+1)+a05(i,j+1)+a08(i,j+1) &
                 +a10(i,j+1)+a12(i,j+1)+a14(i,j+1)+a17(i,j+1)+a19(i,j+1)
!
           d_down=a01(i,j-1)+a03(i,j-1)+a05(i,j-1)+a08(i,j-1) &
                 +a10(i,j-1)+a12(i,j-1)+a14(i,j-1)+a17(i,j-1)+a19(i,j-1)
!
           d_left=a01(i-1,j)+a03(i-1,j)+a05(i-1,j)+a08(i-1,j) &
                 +a10(i-1,j)+a12(i-1,j)+a14(i-1,j)+a17(i-1,j)+a19(i-1,j)
!
           d_right=a01(i+1,j)+a03(i+1,j)+a05(i+1,j)+a08(i+1,j) &
                 +a10(i+1,j)+a12(i+1,j)+a14(i+1,j)+a17(i+1,j)+a19(i+1,j)
!
           up1 =  a01(i,j+1)+a03(i,j+1)+a05(i,j+1) &
                 -a10(i,j+1)-a12(i,j+1)-a14(i,j+1)/d_up
!
          down1 = a01(i,j-1)+a03(i,j-1)+a05(i,j-1) &
                 -a10(i,j-1)-a12(i,j-1)-a14(i,j-1)/d_down
!
          left1 = a03(i-1,j)+a08(i-1,j)+a12(i-1,j) &
                 -a01(i-1,j)-a10(i-1,j)-a17(i-1,j)/d_left
!
          right1= a03(i+1,j)+a08(i+1,j)+a12(i+1,j) &
                 -a01(i+1,j)-a10(i+1,j)-a17(i+1,j)/d_right

!
! delta_x = delta_y = 2.0
              vorticity = 0.5*(up1-down1) - 0.5*(right1-left1)  
              write(55,1004) vorticity
!
           end do
        end do
!
        close(55)
!        
        write(6,*)  "I/O : vorticity (vtk) done"
        write(16,*) "I/O : vorticity (vtk) done"
!
#ifdef DEBUG_1
        if(myrank == 0) then
           write(6,*) "DEBUG1: Exiting from sub. vtk_om"
        endif
#endif
!
1004    format((e14.6,1x))
4000    format(i8.8)
!
       end subroutine vtk_om

