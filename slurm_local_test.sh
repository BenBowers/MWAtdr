#!/bin/bash -l

#SBATCH --partition=workq
#SBATCH --account=mwavcs
#SBATCH --job-name=local_test
#SBATCH --nodes=1
#SBATCH --time=00:01:00
#SBATCH --export=none

module load singularity
export pawseyRepository=/astro/mwavcs/capstone/
export containerImage=$pawseyRepository/local-test.sif

srun --export=all singularity exec --pwd=/app -B $pawseyRepository:/tmp $containerImage $ROOT/app/build/local_test
