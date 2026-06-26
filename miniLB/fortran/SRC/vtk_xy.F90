!     ****** LBE/vtk_xy
!
!     COPYRIGHT
!       (c) 2009 by CASPUR/G.Amati
!       (c) 2000-2011 by CASPUR/G.Amati
!       (c) 2013-20?? by CINECA/G.Amati
!     NAME
!       vtk_xy
!     DESCRIPTION
!       Graphic subroutine:
!       write ASCII output for VTK with velocity + pressure field
!       write on unit 52 (vtk_xy.xxxxxx.dat, where xxxxxxx is the timestep)
!       the file is closed at the end of the subroutine
!     INPUTS
!       itime   ---> timestep
!     OUTPUT
!       none
!     TODO
!       
!     NOTES
!       character used:  file_name (19)
!       integer variables used: itime,i,k, x_scale, z_scale
!       max time allowed  99'999'999
!       single precision only, to saving space..
!
!     *****
!=======================================================================
!
        subroutine vtk_xy(itime)
!
        use storage
        implicit none
!
        integer :: i,j,itime
!	
        character(len=24) :: file_name
!
        real(sp) :: u,w,den
        real(sp) :: cte1

!
        file_name = 'tec_xy.xxxxxxxx.vtk'
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
        open(52,file=file_name,status='unknown')
!
        write(52,'(A26)')'# vtk DataFile Version 2.0'
        write(52,'(A5)')'Campo'
        write(52,'(A5)')'ASCII'
!        write(52,'(A5)')'BINARY'
        write(52,'(A24)')'DATASET RECTILINEAR_GRID'
        write(52,'(A11,I10,A1,I10,A1,I10)')  'DIMENSIONS ',l,' ',m,' ',1
!
        write(52,'(A14,I10,A7)')'X_COORDINATES ',l,' double'
!        do i = 1,l
!           write(52, *) i + offset(1)
!        enddo
!
        write(52,'(A14,I10,A7)')'Y_COORDINATES ',m,' double'
!        do j = 1,m
!           write(52, *) j + offset(2)
!        enddo
!
        write(52,'(A14,I10,A7)')'Z_COORDINATES ',1,' double'
        write(52, *) 0
!
        write(52,'(A10,I10)')'POINT_DATA ',l*m*1
        write(52,'(A24)')'VECTORS velocity double'
!        write(52,'(A20)')'LOOKUP_TABLE default'
        do j = 1,m
           do i = 1,l

              u = a01(i,j)+a03(i,j)+a05(i,j) &
                 -a10(i,j)-a12(i,j)-a14(i,j)
!
              w = a03(i,j)+a08(i,j)+a12(i,j) &
                 -a01(i,j)-a10(i,j)-a17(i,j)
!
              den = a01(i,j)+a03(i,j)+a05(i,j)+a08(i,j) &
                   +a10(i,j)+a12(i,j)+a14(i,j)+a17(i,j)+a19(i,j)+cte1
!
              write(52,1004) u/den, w/den, 0.0
              write(6,*) i,j, u, w,den
           end do
        end do
!
        write(52,'(A21)')'SCALARS u double'
        write(52,'(A20)')'LOOKUP_TABLE default'
        do j = 1,m
           do i = 1,l

              u = a01(i,j)+a03(i,j)+a05(i,j) &
                 -a10(i,j)-a12(i,j)-a14(i,j)
!
              den = a01(i,j)+a03(i,j)+a05(i,j)+a08(i,j) &
                   +a10(i,j)+a12(i,j)+a14(i,j)+a17(i,j)+a19(i,j)+cte1

              write(52,1005) u/den
           end do
        end do
!
        write(52,'(A21)')'SCALARS w double'
        write(52,'(A20)')'LOOKUP_TABLE default'
        do j = 1,m
           do i = 1,l
!
              w = a03(i,j)+a08(i,j)+a12(i,j) &
                 -a01(i,j)-a10(i,j)-a17(i,j)
!
              den = a01(i,j)+a03(i,j)+a05(i,j)+a08(i,j) &
                   +a10(i,j)+a12(i,j)+a14(i,j)+a17(i,j)+a19(i,j)+cte1

              write(52,1005) w/den
           end do
        end do
!
        write(52,'(A21)')'SCALARS rho double'
        write(52,'(A20)')'LOOKUP_TABLE default'
        do j = 1,m
           do i = 1,l
!
              den = a01(i,j)+a03(i,j)+a05(i,j)+a08(i,j) &
                   +a10(i,j)+a12(i,j)+a14(i,j)+a17(i,j)+a19(i,j)+cte1
!
              write(52,1005) den
           end do
        end do
!
        close(52)
        write(6,*)  "I/O : plane xy (vtk) done"
        write(16,*) "I/O : plane xy (vtk) done"
!
#ifdef DEBUG_1
        if(myrank == 0) then
           write(6,*) "DEBUG1: Exiting from sub. vtk_xy"
        endif
#endif

! formats
1004    format(3(e14.6,1x))
1005    format(1(e14.6,1x))
4000    format(i8.8)

       end subroutine vtk_xy

