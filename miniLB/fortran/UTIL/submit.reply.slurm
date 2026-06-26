#!/bin/bash
# 
#!/bin/bash
#
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=8
#SBATCH --gres=gpu:1
#SBATCH -p boost_usr_prod
#SBATCH -t 02:00:00
#SBATCH --exclusive

# setup environment

module purge
module load nvhpc
module li

for ver in offload openacc doconcurrent; do
    echo $ver
    mkdir $ver-reply-orig-512
    cd $ver-reply-orig-512
    for iter in `seq 1 10`; do
	echo $iter
	mkdir $iter
	cd $iter
	cp ../../bgk.512.R1000.input bgk.input
	cp ../../bgk2d.$ver.x bgk2d.$ver.x
        ./bgk2d.$ver.x > out.log
	cd ..
    done    
    cd ..
done    



