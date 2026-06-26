#!/bin/bash
#
#SBATCH --nodes=1
#SBATCH --ntasks=4
#SBATCH --ntasks-per-node=4
#SBATCH --gres=gpu:4 
#SBATCH -p  m100_sys_test\
#SBATCH -A cin_staff
#SBATCH -t 4:00:00 
#SBATCH --exclusive
#SBATCH --qos=qos_test
#
module load hpc-sdk
module li 
#
mkdir $SLURM_JOBID
cd $SLURM_JOBID
#
env > env.log
scontrol show hostname $SLURM_NODELIST > hostlist.txt 
#
for dir in 256 512 1024; do
    echo running $dir
    mkdir $dir
    cd  $dir
    cp ../../bgk2d.offload.x .
    cp ../../bgk.$dir.* bgk.input
    ./bgk2d.offload.x > output.log
    cd -
done
#
