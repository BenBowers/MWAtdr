#!/bin/bash -l

#SBATCH --partition=workq
#SBATCH --account=mwavcs
#SBATCH --job-name=local_test
#SBATCH --nodes=1
#SBATCH --time=00:01:00
#SBATCH --export=none

module load singularity-openmpi

if [[ $# -ne 6 ]] ; then
    echo "Usage: sbatch slurm_main.sh <inputDir> <obsId> <startTime> <invPolyphaseFilterFile> <outputDir> <ignoreErrors>"
    exit 1
fi

hostInputDir=$(realpath -m $1)
obsId=$2
startTime=$3
hostInvPolyphaseFilterFile=$(realpath -m $4)
hostOutputDir=$(realpath -m $5)
ignoreErrors=$6
export pawseyRepository=/astro/mwavcs/capstone/
export containerImage=$pawseyRepository/local-test.sif
export repoDir = /astro/mwavcs/capstone/repo/

export hostInputDir=$(realpath -m $1)
export obsId=$2
export startTime=$3
export hostInvPolyphaseFilterFile=$(realpath -m $4)
export hostOutputDir=$(realpath -m $5)
export ignoreErrors=$6

export containerInputDir="/mnt/input_data"
export containerInvPolyphaseFilterFile="/mnt/inverse_polyphase_filter"
export containerOutputDir="/mnt/output_data"

srun -n --export=all singularity exec --pwd=/app\
     -B $hostInputDir:$containerInputDir:ro,\
        $hostOutputDir:$containerOutputDir:rw,\
        $hostInvPolyphaseFilterFile:$containerInvPolyphaseFilterFile:ro \
    $containerImage $ROOT/app/entrypoint.sh $containerInputDir $obsId $startTime $containerInvPolyphaseFilterFile $containerOutputDir $ignoreErrors
