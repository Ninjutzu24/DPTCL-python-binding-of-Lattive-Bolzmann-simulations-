#!/bin/tcsh
#
echo $1
setenv COMP $1
#
setenv DIR RUN_SINGLE_SERIAL_FUSED_$COMP
setenv EXE bgk2d.serial.x
#
echo "---------------------------"
echo "starting test driven cavity"
echo " ---> single precision     "
echo " ---> fused                "
echo " ---> serial              "
echo " ---> " $EXE
echo " ---> " $DIR
echo " ---> " $COMP
echo "---------------------------"
#
rm -rf $DIR
mkdir $DIR
cd $DIR

# step 1: compiling
echo "step 1: compiling"
cd ../../../SRC
make clean
make serial $COMP SINGLE=1 FUSED=1 LDC=1
if ($?) then
   echo "compiling fails..."
   exit 1
else
   cd -
   cp ../../../RUN/$EXE  .
   cp ../../../UTIL/bgk.core.input  bgk.input
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

