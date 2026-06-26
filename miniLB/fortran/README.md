BGK2D_GPU is a open-source software foe 2D single phase incompressible flows.
The code was orignally written and developed by:

* Giorgio Amati		CINECA, Rome branch	Italy

This is an experimental code. The author accept no responsibility
for the performance of the code or for the correctness of the results.

The code is licensed under MIT Software License 


------------------------------------------------------------------------------
1) Description

This is a 2D CFD Lattice Boltzmann Method code developed to 
	1) Give a (quite) efficient implementation of LBM
 	2) Exploit mulithreading
  	3) assess performance using (different) GPUs 

It is a "downsize" of a more complex code (3D one with mpi+openacc). 
Different options can be activated/decativated via preprocessing flags

You can choose

	* 2 different implementations 
  		* ORIGINAL (default): Classical implementation, with two different subroutine streaming + collision
  		* FUSED Implementation with one single fused subroutine (no streaming subroutine).  It asks for less BW but it uses pointers.

	* 5 different exe
  		* SERIAL 
  		* DO_CONCURRENT
  			* for CPU 
			* for GPU  (default)
  		* OPENMP OFFLOAD 
			* GPU (NVIDIA and AMD)
  		* OPENACC
   			* GPU 

	* 5 different tested compilers
		* GNU (gfortran): default
  		* NVIDIA (nvfortran)
  		* INTEL (ifx)
  		* AMD (flang)
  		* CRAY (ftn)
 

	* 4 different precision 
  		* SINGLE: single precision (default)
  		* DOUBLE: double precision
  		* MIXED1: mixed precision1 (half/single)
  		* MIXED2: mixed precision2 (single/double)

	* 4 different test cases
		* Lid Driven Cavity (default)
		* Poiseuille flow
		* taylor-green vortex
		* Von Karman street

To be noted that not all possible combinations are supported (e.g. GNU doesn't support half precision)

------------------------------------------------------------------------------
2) Directory Structure

.
├── BENCH
├── CI
├── DOC
├── FIX
├── MIT
├── README.md
├── SRC
├── TEST
└── UTIL

where:
	* DOC directory: some documentation, in pdf format
	* SRC directory: all source files + Makefile. The exe will be copied in ../RUN directory (to create)
	* UTIL directory: some utilities like scripts or input file
	* TEST directory: four test case
	  * lid-driven cavity
	  * Taylor-green vortex
	  * flow around an obstacle (cylinder: von Karman streets)
	  * poiseuille flow
	* BENCH directory: script and input file for bechmarking at core, node and GPU level
	* CI: an hand-made script to perform compilation of all possible combination between compiler, precision, implementation, test case and targets (still under development)

------------------------------------------------------------------------------
3) How to compile

the sintax is (in the SRC directory)

make <target> <precision>=1 <compiler>=1 <version>=1 w<testcase>=1 

where 
	* <target>=serial,multicore,offload,openacc,doconcurrent (default)
	* <precision>=mixed1,single (default),mixed2,double
	* <compiler>=INTEL,CRAY,AMD,NVIDIA,GNU (default)
	* <testcase>=POF,TGV,VKS,LDC(default) where
		* POF= poiseuille flow
		* TGV= Taylor Green Vortex
		* LDC= lid-driven cavity
		* VKS= Von karman Street
	

4) Other stuff

Other preprocessing flags are:

* TRICK1 (to reduce the impact of BC on performance --> square box")
* DRAG (activate drag/lift computing, time-consuming)

5) Licence

This SW is under The MIT licence

