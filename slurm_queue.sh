#!/bin/bash -l

#SBATCH --partition=workq
#SBATCH --account=mwavcs
#SBATCH --job-name=mpi_test
#SBATCH --ntasks=10
#SBATCH --ntasks-per-node=10
#SBATCH --time=00:01:00
#SBATCH --export=none

module load singularity

export pawseyRepository=/astro/mwavcs/capstone/
export containerImage=$pawseyRepository/mpi_test_latest.sif

srun --export=all -n $SLURM_NTASKS singularity exec $containerImage $ROOT/app/build/mpi_test
