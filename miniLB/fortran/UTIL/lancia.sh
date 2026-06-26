module load compilers/nvhpc/2021
module li 
#
env > env.log
#
for test in 1 2 3 4 5; do
mkdir $test
cd $test
echo running $test
  for dir in 256 512 1024; do
    echo running $dir
    mkdir $dir
    cd  $dir
    cp ../../bgk2d.doconcurrent.x .
    cp ../../bgk.$dir.* bgk.input
    ./bgk2d.doconcurrent.x > output.log
    cd ..
  done
  cd ..
done
#
