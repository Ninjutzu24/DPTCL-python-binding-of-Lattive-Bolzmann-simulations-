#!/bin/bash
#SBATCH --job-name=prova
#SBATCH --account=cca4298
#SBATCH --constraint=MI250
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=8 --cpus-per-task=8 --gpus-per-node=8
#SBATCH --threads-per-core=1 # --hint=nomultithread
#SBATCH --exclusive
#SBATCH --output=%A.out
#SBATCH --time=00:30:00

module purge

#rocm module
module load rocmmod5.3.0

module li

mkdir $SLURM_JOBID
cd $SLURM_JOBID

export OMP_NUM_THREADS=1

env > env.out

cp ../bgk.input .
cp ../bgk2d.offload.x .

./bgk2d.offload.x > out.log
