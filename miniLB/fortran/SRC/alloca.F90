!=====================================================================
!     ****** LBE/alloca
!
!     COPYRIGHT
!       (c) 2021 by CINECA/G.Amati
!     NAME
!       alloca
!     DESCRIPTION
!
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
      subroutine alloca()
!            
      use storage; 
      use timing
! 
      implicit none
!
!     define unit vector  (real/integer)
!
      cx(  1    ) =  1
      cx(  2    ) =  1
      cx(  3    ) =  1
      cx(  4    ) =  1
      cx(  5    ) =  1
      cx(  6    ) =  0
      cx(  7    ) =  0
      cx(  8    ) =  0
      cx(  9    ) =  0
      cx( 10    ) = -1
      cx( 11    ) = -1
      cx( 12    ) = -1
      cx( 13    ) = -1
      cx( 14    ) = -1
      cx( 15    ) =  0
      cx( 16    ) =  0
      cx( 17    ) =  0
      cx( 18    ) =  0
      cx( 19    ) =  0

      icx(  1    ) =  1
      icx(  2    ) =  1
      icx(  3    ) =  1
      icx(  4    ) =  1
      icx(  5    ) =  1
      icx(  6    ) =  0
      icx(  7    ) =  0
      icx(  8    ) =  0
      icx(  9    ) =  0
      icx( 10    ) = -1
      icx( 11    ) = -1
      icx( 12    ) = -1
      icx( 13    ) = -1
      icx( 14    ) = -1
      icx( 15    ) =  0
      icx( 16    ) =  0
      icx( 17    ) =  0
      icx( 18    ) =  0
      icx( 19    ) =  0
!
      cy(  1    ) = -1
      cy(  2    ) =  0
      cy(  3    ) =  1
      cy(  4    ) =  0
      cy(  5    ) =  0
      cy(  6    ) =  0
      cy(  7    ) =  1
      cy(  8    ) =  1
      cy(  9    ) =  1
      cy( 10    ) = -1
      cy( 11    ) =  0
      cy( 12    ) =  1
      cy( 13    ) =  0
      cy( 14    ) =  0
      cy( 15    ) =  0
      cy( 16    ) = -1
      cy( 17    ) = -1
      cy( 18    ) = -1
      cy( 19    ) =  0
!      
      icy(  1    ) = -1
      icy(  2    ) =  0
      icy(  3    ) =  1
      icy(  4    ) =  0
      icy(  5    ) =  0
      icy(  6    ) =  0
      icy(  7    ) =  1
      icy(  8    ) =  1
      icy(  9    ) =  1
      icy( 10    ) = -1
      icy( 11    ) =  0
      icy( 12    ) =  1
      icy( 13    ) =  0
      icy( 14    ) =  0
      icy( 15    ) =  0
      icy( 16    ) = -1
      icy( 17    ) = -1
      icy( 18    ) = -1
      icy( 19    ) =  0
!
      allocate(obs(1:l,1:m))
      obs = 0
!
      allocate(a01(0:l+1+ipad,0:m+1+jpad))
      allocate(a03(0:l+1+ipad,0:m+1+jpad))
      allocate(a05(0:l+1+ipad,0:m+1+jpad))
      allocate(a08(0:l+1+ipad,0:m+1+jpad))
      allocate(a10(0:l+1+ipad,0:m+1+jpad))
      allocate(a12(0:l+1+ipad,0:m+1+jpad))
      allocate(a14(0:l+1+ipad,0:m+1+jpad))
      allocate(a17(0:l+1+ipad,0:m+1+jpad))
      allocate(a19(0:l+1+ipad,0:m+1+jpad))
!
      allocate(b01(0:l+1+ipad,0:m+1+jpad))
      allocate(b03(0:l+1+ipad,0:m+1+jpad))
      allocate(b05(0:l+1+ipad,0:m+1+jpad))
      allocate(b08(0:l+1+ipad,0:m+1+jpad))
      allocate(b10(0:l+1+ipad,0:m+1+jpad))
      allocate(b12(0:l+1+ipad,0:m+1+jpad))
      allocate(b14(0:l+1+ipad,0:m+1+jpad))
      allocate(b17(0:l+1+ipad,0:m+1+jpad))
      allocate(b19(0:l+1+ipad,0:m+1+jpad))
!
      a01 = huge(mykind)
      a03 = huge(mykind)
      a05 = huge(mykind)
      a08 = huge(mykind)
      a10 = huge(mykind)
      a12 = huge(mykind)
      a14 = huge(mykind)
      a17 = huge(mykind)
      a19 = huge(mykind)
!      
      b01 = huge(mykind)
      b03 = huge(mykind)
      b05 = huge(mykind)
      b08 = huge(mykind)
      b10 = huge(mykind)
      b12 = huge(mykind)
      b14 = huge(mykind)
      b17 = huge(mykind)
      b19 = huge(mykind)
!
#ifdef FUSED
      c01 => null()
      c03 => null()
      c05 => null()
      c08 => null()
      c10 => null()
      c12 => null()
      c14 => null()
      c17 => null()
      c19 => null()
#endif

#ifdef PGI
! do nothing
#else
      mem_start = get_mem()
#endif

#ifdef DEBUG_1
      if(myrank == 0) then
         write(6,*) "DEBUG1: Exiting from sub. alloca"
      endif
#endif
!
# ifdef MEM_CHECK
      if(myrank == 0) then
         mem_stop = get_mem();
         write(6,*) "MEM_CHECK: after sub. alloca mem =", mem_stop
      endif
# endif

      end subroutine alloca

