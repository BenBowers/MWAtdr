#!/bin/bash -l

#SBATCH --partition=workq
#SBATCH --account=mwavcs
#SBATCH --job-name=mwatdr
#SBATCH --ntasks=128
#SBATCH --ntasks-per-node=8
#SBATCH --mem=128G
#SBATCH --time=00:05:00
#SBATCH --export=none

module load singularity-openmpi

if [[ $# -ne 6 ]] ; then
    echo "Usage: sbatch slurm_main.sh <inputDir> <obsId> <startTime> <invPolyphaseFilterFile> <outputDir> <ignoreErrors>"
    exit 1
fi

export pawseyRepository=/astro/mwavcs/capstone/
export containerImage=$pawseyRepository/images/main.sif

export hostInputDir=$(realpath -m $1)
export obsId=$2
export startTime=$3
export hostInvPolyphaseFilterFile=$(realpath -m $4)
export hostOutputDir=$(realpath -m $5)
export ignoreErrors=$6

export containerInputDir=/mnt/input_data
export containerInvPolyphaseFilterFile=/mnt/inverse_polyphase_filter
export containerOutputDir=/mnt/output_data

srun --export=all -n $SLURM_NTASKS  singularity exec --pwd=/app \
     --bind $hostInputDir:$containerInputDir:ro,$hostOutputDir:$containerOutputDir:rw,$hostInvPolyphaseFilterFile:$containerInvPolyphaseFilterFile:ro \
    $containerImage $ROOT/app/entrypoint.sh $containerInputDir $obsId $startTime $containerInvPolyphaseFilterFile $containerOutputDir $ignoreErrors
