for ver in doconcurrent offload openacc; do
    echo $ver
    mkdir $ver
    cd $ver
    for size in bgk.256.R100.input bgk.512.R1000.input bgk.1024.R10000.input bgk.2048.R10000.input bgk.2912.R10000.input ; do
	echo $size
	mkdir $size
	cd $size
	cp ../../$size bgk.input
	cp ../../bgk2d.$ver.x bgk2d.$ver.x
        ./bgk2d.$ver.x > out.log
	cd ..
    done    
    cd ..
done    
