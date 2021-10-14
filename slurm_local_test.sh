#!/bin/bash -l

#SBATCH --partition=workq
#SBATCH --account=mwavcs
#SBATCH --job-name=mwatdr/local_test
#SBATCH --nodes=1
#SBATCH --time=00:01:00
#SBATCH --export=none

module load singularity

export pawseyRepository=/astro/mwavcs/capstone/
export containerImage=$pawseyRepository/images/local-test.sif

export repoDir = /astro/mwavcs/capstone/repo/

export hostTempDir="$(pawseyRepository)/tmp/"
export hostInputDir="$(repoDir)/test/input_data"
export hostOutputDir="$(repoDir)/test/output_data"
export containerInputDir="/mnt/test_input"
export containerOutputDir="/mnt/test_output"

srun --export=all singularity exec --pwd=/app\
     -B $pawseyRepository:/tmp:rw,$hostInputDir:$containerInputDir:ro,$hostOutputDir:$containerOutputDir:rw \
    $containerImage $ROOT/app/build/local_test
