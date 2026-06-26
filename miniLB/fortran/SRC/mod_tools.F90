module timing

      use real_kinds
!
!
#ifdef PGI
! do nothing
#else
      use check_mem
#endif
!
      implicit none
!
      REAL(sp):: time_init, time_init1
      REAL(sp):: time_coll, time_coll1
      REAL(sp):: time_loop, time_loop1
      REAL(sp):: time_move, time_move1
      REAL(sp):: time_obs, time_obs1
      REAL(sp):: time_bc, time_bc1
      REAL(sp):: time_mp, time_mp1
      REAL(sp):: time_dg, time_dg1
      REAL(sp):: time_dev, time_dev1
      REAL(sp):: time_io, time_io1
      REAL(sp):: time_inn_loop, time_inn_loop1
      REAL(sp):: timeZ, timeY, timeX
!
      INTEGER :: count_rate, count_max
      INTEGER :: count1, count0
      INTEGER :: count2, count3
      INTEGER :: countA0, countB0, countC0, countD0, countE0, countF0, countO0
      INTEGER :: countA1, countB1, countC1, countD1, countE1, countF1, countO1
      INTEGER :: countH0
      INTEGER :: countH1
!
      REAL(mykind) :: tcount1, tcount0
      REAL(mykind) :: tcount2, tcount3
      REAL(sp) :: tcountA0,tcountB0,tcountC0            ! single prec.
      REAL(sp) :: tcountD0,tcountE0,tcountF0            ! single prec.
      REAL(sp) :: tcountG0,tcountH0                     ! single prec.
      REAL(sp) :: tcountO0                              ! single prec.
      REAL(sp) :: tcountA1,tcountB1,tcountC1            ! single prec.
      REAL(sp) :: tcountD1,tcountE1,tcountF1            ! single prec.
      REAL(sp) :: tcountG1,tcountH1                     ! single prec.
      REAL(sp) :: tcountO1                              ! single prec.
      REAL(sp) :: old1, old2, old3, old4, old5, old6    ! single prec.
      REAL(sp) :: tcountZ0,tcountZ1
      REAL(sp) :: tcountY0,tcountY1
      REAL(sp) :: tcountX0,tcountX1
      REAL(dp) :: mem_start, mem_stop                  ! double precision.
!
contains
subroutine time(t)

      real(sp), intent(out) :: t
      integer :: time_array(8)

call date_and_time(values=time_array)
t = 3600.*time_array(5)+60.*time_array(6)+time_array(7)+time_array(8)/1000.

end subroutine time

end module timing

