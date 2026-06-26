#!/bin/tcsh
#
# trick for leonardo ....
module purge
module load nvhpc/23.1
module li
#
echo "WARNING: rel 23.1 used....."
setenv DIR RUN_SINGLE_FUSED_OFFLOAD_NV
setenv EXE bgk2d.offload.x
#
echo "-------------------------------"
echo "starting test VonKarman Street " 
echo " ---> nvfortran            "
echo " ---> single precision     "
echo " ---> fused                "
echo " ---> openacc              "
echo " ---> "$EXE
echo " ---> "$DIR
echo "-------------------------------"
#
rm -r $DIR
mkdir $DIR
cd $DIR

# step 1: compiling
echo "step 1: compiling"
cd ../../../SRC
make clean
make offload FIX="-DFUSED -DINFLOW -DOBSTACLE -DDRAG"
if ($?) then
   echo "compiling fails..."
   exit 1
else
   cd -
   cp ../../../RUN/$EXE  .
   cp ../bgk.input .
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

