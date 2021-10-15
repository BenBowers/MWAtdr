#!/bin/bash -l

#SBATCH --partition=workq
#SBATCH --account=mwavcs
#SBATCH --job-name=mpi_test
#SBATCH --ntasks=30
#SBATCH --ntasks-per-node=2
#SBATCH --time=00:05:00
#SBATCH --export=none

module load singularity-openmpi
export pawseyRepository=/astro/mwavcs/capstone/
export containerImage=$pawseyRepository/mpi-test.sif

srun --export=all -n $SLURM_NTASKS singularity exec --pwd=/app $containerImage $ROOT/app/test/mpi/run.sh
