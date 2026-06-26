!=======================================================================
!     ****** LBE/vtk_om_bin
!
!     COPYRIGHT
!       (c) 2009 by CASPUR/G.Amati
!       (c) 2000-2011 by CASPUR/G.Amati
!       (c) 2013-20?? by CINECA/G.Amati
!     NAME
!       vtk_om_bin
!     DESCRIPTION
!       Graphic subroutine:
!       write binary output for VTK with vorticity (stream field to do)
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
!       max task allowed  9999
!       single precision only, to saving space..
!
!     *****
!=======================================================================
!
        subroutine vtk_om_bin(itime)
!
        use storage
        implicit none
!
        integer :: i,j,itime
!	
        character(len=24) :: file_name
!
        real(sp) :: den, uv, vv
        real(sp) :: up1, down1, left1, right1
        real(sp) :: d_up, d_down, d_left, d_right
        real(sp) :: vorticity
        real(sp) :: phi(-1:m1)
        real(sp) :: cte1
!
        file_name = 'tec_om.xxxxxxxx.vtk'
!
        myrank = 0
!
#ifdef NOSHIFT
       cte1 = zero
#else
       cte1 = uno
#endif
!
        write(file_name(8:15),4000) itime
        open(55,file=file_name,status='unknown')

!for now only vorticity is computed
!
! pre-compute phi ...
        phi(-1) = 0.0
        i = 1
        do j = 0, m
           den =( a01(i,j)+a03(i,j)+a05(i,j)+a08(i,j) & 
                 +a10(i,j)+a12(i,j)+a14(i,j)+a17(i,j)+a19(i,j)) + cte1

           uv =   a01(i,j)+a03(i,j)+a05(i,j) &
                 -a10(i,j)-a12(i,j)-a14(i,j)
!
           phi(j) =  phi(j-1) + uv/den
        enddo
!
! vtk header
        write(55,'(A26)')'# vtk DataFile Version 2.0'
        write(55,'(A5)')'Campo'
        write(55,'(A6)')'BINARY'
        write(55,'(A25)')'DATASET STRUCTURED_POINTS'
        write(55,'(A11,I10,A1,I10,A1,I10)') &
                              'DIMENSIONS ',l-2,' ',m-2,' ',1
        write(55,'(A7,I10,A1,I10,A1,I10)')  'ORIGIN ',offset(1)+2,' ' &
                                                     ,offset(2)+2,' ' &
                                                     ,1
        write(55,'(A8,I10,A1,I10,A1,I10)') 'SPACING ',1,' ',1,' ',1
        write(55,'(A10,I10)')'POINT_DATA ',(l-2)*(m-2)
!
! vorticity (scalar)
        write(55,'(A24)')'SCALARS vorticity float'
        write(55,'(A20)')'LOOKUP_TABLE default'
        close(55)
!
! then write output (binary)
        open(55,file=file_name,status='old', position='append', &
                form='unformatted',access='STREAM',CONVERT="BIG_ENDIAN")
!
        do j = 2,m-1
           do i = 2,l-1
!
           d_up=   (a01(i,j+1)+a03(i,j+1)+a05(i,j+1)+a08(i,j+1) &
                   +a10(i,j+1)+a12(i,j+1)+a14(i,j+1)+a17(i,j+1)+a19(i,j+1)) + cte1
!
           d_down= (a01(i,j-1)+a03(i,j-1)+a05(i,j-1)+a08(i,j-1) &
                   +a10(i,j-1)+a12(i,j-1)+a14(i,j-1)+a17(i,j-1)+a19(i,j-1)) + cte1
!
           d_left= (a01(i-1,j)+a03(i-1,j)+a05(i-1,j)+a08(i-1,j) &
                   +a10(i-1,j)+a12(i-1,j)+a14(i-1,j)+a17(i-1,j)+a19(i-1,j)) + cte1
!
           d_right=(a01(i+1,j)+a03(i+1,j)+a05(i+1,j)+a08(i+1,j) &
                   +a10(i+1,j)+a12(i+1,j)+a14(i+1,j)+a17(i+1,j)+a19(i+1,j)) + cte1
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
              write(55) vorticity
!
           end do
        end do
        close(55)
!
! stream (scalar)
        open(55,file=file_name,status='old', position='append')
        write(55,'(A25)')'SCALARS stream float'
        write(55,'(A20)')'LOOKUP_TABLE default'
        close(55)
!        
! then write output (binary)
        open(55,file=file_name,status='old', position='append', &
                form='unformatted',access='STREAM',CONVERT="BIG_ENDIAN")
        do j = 2,m-1
           do i = 2,l-1
!
              den =( a01(i,j)+a03(i,j)+a05(i,j)+a08(i,j) & 
                   +a10(i,j)+a12(i,j)+a14(i,j)+a17(i,j)+a19(i,j)) + cte1
!
              vv  = a03(i,j)+a08(i,j)+a12(i,j) & 
                   -a01(i,j)-a10(i,j)-a17(i,j)
!
               phi(j) = phi(j) - vv/den
!
              write(55) phi(j)
           end do
        end do
        close(55)
!
        write(6,*)  "I/O: vorticity xy (vtk) done"
        write(16,*) "I/O: vorticity xy (vtk) done"
!
#ifdef DEBUG_1
        if(myrank == 0) then
           write(6,*) "DEBUG1: Exiting from sub. vtk_om_bin"
        endif
#endif
!
4000    format(i8.8)
!
       end subroutine vtk_om_bin

