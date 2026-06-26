#!/bin/tcsh
#
setenv DIR RUN_DOUBLE_SERIAL_FUSED_GNU
setenv EXE bgk2d.serial.x
#
echo "---------------------------"
echo "starting test driven cavity"
echo " ---> gfrotran             "
echo " ---> single precision     "
echo " ---> fused                "
echo " ---> serial               "
echo " ---> "$EXE
echo " ---> "$DIR
echo "---------------------------"
#
rm -rf $DIR
mkdir $DIR
cd $DIR

# step 1: compiling
echo "step 1: compiling"
cd ../../../SRC
make clean
make serial GNU=1 FUSED=1 DOUBLE=1 LDC=1
if ($?) then
   echo "compiling fails..."
   exit 1
else
   cd -
   cp ../../../RUN/$EXE  .
   cp ../../../UTIL/bgk.128*  bgk.input
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

