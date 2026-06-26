export INFO=$1
export EXE=bgk2d.offload.x
echo "Running " $EXE, "with this input",  $INFO
for size in bgk.256.R100.input bgk.512.R1000.input bgk.1024.R10000.input; do
    echo "running", $size
    mv $size bgk.input
    ./clean.sh
    ./$EXE > output.$size.$INFO.txt
    mv bgk.perf $size.$INFO.perf
done




