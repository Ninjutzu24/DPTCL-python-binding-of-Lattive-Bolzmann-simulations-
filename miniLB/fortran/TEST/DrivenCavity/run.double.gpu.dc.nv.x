#!/bin/tcsh
#
setenv DIR RUN_DOUBLE_GPU_FUSED_DC_NV
setenv EXE bgk2d.doconcurrent.x
#
echo "---------------------------"
echo "starting test driven cavity"
echo " ---> nvfortran            "
echo " ---> double precision     "
echo " ---> fused                "
echo " ---> GPU                  "
echo " ---> doconcurrent         "
echo " ---> " $EXE
echo " ---> " $DIR
echo "---------------------------"
#
rm -r $DIR
mkdir $DIR
cd $DIR

# step 1: compiling
echo "step 1: compiling"
cd ../../../SRC
make clean
make FIX="-DFUSED" DOUBLE=1
if ($?) then
   echo "compiling fails..."
   exit 1
else
   cd -
   cp ../../../RUN/$EXE  .
   cp ../../../UTIL/bgk.512*  bgk.input
   echo "compiling  ended succesfully..."
endif


# step 2: running test
echo "step 2: running test"
./$EXE >& out.log
if (-e "bgk.perf") then
   echo "run ended succesfully..."
else
   echo "running test fails..."
   exit 1
endif

