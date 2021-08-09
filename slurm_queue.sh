#!/bin/bash -l

#SBATCH --account=projectID
#SBATCH --job-name=####
#SBATCH --ntasks=###
#SBATCH --ntasks-per-node=##
#SBATCH --time=00:01:00
#SBATCH --export=none

module load singularity

export pawseyRepository= # Pawsey repo here
export containerImage=$pawseyRepository/ # Singularity image

srun --export=all -n $SLURM_NTASKS singularity exec $containerImage # rest of singularity args
