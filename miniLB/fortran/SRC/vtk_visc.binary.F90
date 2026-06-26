!=======================================================================
!     ****** LBE/vtk_visc_bin
!
!     COPYRIGHT
!       (c) 2009 by CASPUR/G.Amati
!       (c) 2000-2011 by CASPUR/G.Amati
!       (c) 2013-20?? by CINECA/G.Amati
!     NAME
!       vtk_visx_bin
!     DESCRIPTION
!       Graphic subroutine:
!       write 2D binary output for VTK with "effective viscosity"
!       write on unit 52 (vtk_visc.xxxxxxx.dat) where 
!                                  xxxxxxx is the timestep
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
        subroutine vtk_visc_bin(itime)
!
        use storage
        implicit none
!
        integer :: i,j,itime
!	
        character(len=24) :: file_name
!
        real(sp) :: u,w,den
        real(sp) :: cte1, cte0
        real(sp), parameter :: zerol=0.0
        real(sp) :: x01,x03,x05,x08,x10
        real(sp) :: x12,x14,x17,x19
        real(sp) :: e01,e03,e05,e08,e10
        real(sp) :: e12,e14,e17,e19
        real(sp) :: n01,n03,n05,n08,n10
        real(sp) :: n12,n14,n17,n19
        real(sp) :: rho,rhoinv,vx,vy,vx2,vy2,vsq
        real(sp) :: vxpy,vxmy,rp1,rp2,rp0
        real(sp) :: qxpy,qxmy,qx,qy,q0
        real(sp) :: Pxx,Pxy,Pyx,Pyy,Ptotal
        real(sp) :: omega2
        real(sp) :: Ts
!
        file_name = 'tec_visc.xxxxxxxx.vtk'
!
        myrank = 0
!
#ifdef NOSHIFT
        cte1 = zero
#else
        cte1 = uno
#endif
        cte0 = uno - cte1
!
        write(file_name(10:17),4000) itime
!
! first write legal header (ASCII)
        open(52,file=file_name,status='unknown')
!
        write(52,'(A26)')'# vtk DataFile Version 2.0'
        write(52,'(A5)') 'Campo'
        write(52,'(A6)') 'BINARY'
        write(52,'(A25)')'DATASET STRUCTURED_POINTS'
        write(52,'(A11,I10,A1,I10,A1,I10)') 'DIMENSIONS ',l,' ',m,' ',1
        write(52,'(A7,I10,A1,I10,A1,I10)')  'ORIGIN ',offset(1)+1,' ' & 
                                                     ,offset(2)+1,' ' & 
                                                     ,1        
        write(52,'(A8,I10,A1,I10,A1,I10)') 'SPACING ',1,' ',1,' ',1     
        write(52,'(A10,I10)')'POINT_DATA ',l*m*1
!
! vector
        write(52,'(A24)')'SCALARS viscosity float'
        write(52,'(A20)')'LOOKUP_TABLE default'
        close(52)
!
! then write output (binary)
        open(52,file=file_name,status='old', position='append', & 
                form='unformatted',access='STREAM',CONVERT="BIG_ENDIAN")
!
        do j = 1,m
           do i = 1,l
!
              x01 = a01(i,j)
              x03 = a03(i,j)
              x05 = a05(i,j)
              x08 = a08(i,j)
              x10 = a10(i,j)
              x12 = a12(i,j)
              x14 = a14(i,j)
              x17 = a17(i,j)
              x19 = a19(i,j)
!
              rho = (+x01 +x03 +x05 +x08 +x10 +x12 +x14 +x17 +x19) + cte1
!
              vx = (x01+x03+x05-x10-x12-x14)/rho
              vy = (x03+x08+x12-x01-x10-x17)/rho

! Quadratic terms
              vx2 = vx*vx
              vy2 = vy*vy
!
              vsq = vx2+vy2
!
              vxpy = vx+vy
              vxmy = vx-vy
!              
              qxpy = cte0+qf*(tre*vxpy*vxpy-vsq)
              qxmy = cte0+qf*(tre*vxmy*vxmy-vsq)
              qx   = cte0+qf*(tre*vx2      -vsq)
              qy   = cte0+qf*(tre*vy2      -vsq)
              q0   = cte0+qf*(             -vsq)
!
! linear terms
              vx   = rf*vx
              vy   = rf*vy
              vxpy = rf*vxpy
              vxmy = rf*vxmy
!
! constant terms
              rp0 = rho*p0
              rp1 = rho*p1
              rp2 = rho*p2
!              
! equilibrium distribution
              e01 = rp2*(+vxmy+qxmy)+cte1*(rp2-p2)
              e03 = rp2*(+vxpy+qxpy)+cte1*(rp2-p2)
              e05 = rp1*(+vx  +qx  )+cte1*(rp1-p1)
              e08 = rp1*(+vy  +qy  )+cte1*(rp1-p1)
              e10 = rp2*(-vxpy+qxpy)+cte1*(rp2-p2)
              e12 = rp2*(-vxmy+qxmy)+cte1*(rp2-p2)
              e14 = rp1*(-vx  +qx  )+cte1*(rp1-p1)
              e17 = rp1*(-vy  +qy  )+cte1*(rp1-p1)
              e19 = rp0*(     +q0  )+cte1*(rp0-p0)
!
! non-equilibrium distribution
              n01 = x01-e01
              n03 = x03-e03
              n05 = x05-e05
              n08 = x08-e08
              n10 = x10-e10
              n12 = x12-e12
              n14 = x14-e14
              n17 = x17-e17
!
              Pxx = n01 + &
                    n03 + &
                    n05 + &
                    n10 + &
                    n12 + &
                    n14
!
              Pyy = n01 + &
                    n03 + &
                    n08 + &
                    n10 + &
                    n12 + &
                    n17
!
              Pxy = -n01+n03+n10-n12
!
              Pyx = Pxy
!
! calculate Pi total
              Ptotal =sqrt((Pxx)**2 + (2*Pxy*Pyx) + (Pyy)**2)
!
! adding turbulent viscosity
              Ts = 1/(2*omega1) + sqrt(18.0*(cteS**2)*Ptotal + (1.0/omega1)**2)/2
              omega2 = 1/Ts
!
              write(52) (2.0/omega2 -1.0)/6.0

           end do
        end do
        close(52)
!
        write(16,*) "I/O: viscosity (vtk,binary) done"
        write(6,*)  "I/O: viscosity (vtk,binary) done"
!
#ifdef DEBUG_1
        if(myrank == 0) then
           write(6,*) "DEBUG1: Exiting from sub. vtk_visc_bin"
        endif
#endif
!
4000    format(i8.8)
!
       end subroutine vtk_visc_bin
