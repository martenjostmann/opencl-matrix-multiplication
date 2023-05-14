#!/bin/bash

#SBATCH --exclusive
#SBATCH --partition=gpu2080
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=1
#SBATCH --gres=gpu:8
#SBATCH --time=8:00:00
#SBATCH --job-name=matrixmult
#SBATCH --output=/scratch/tmp/m_jost02/gpu2080node1.out
#SBATCH --error=/scratch/tmp/m_jost02/gpu2080node1.error
#SBATCH --mail-type=ALL
#SBATCH --mail-user=m_jost02@uni-muenster.de
#SBATCH --mem=0

partition=cpu
implementation="sequential"
dirname=$(date +"%Y-%m-%dT%H-%M-%S-${partition}-${implementation}")

module --force purge
ml palma/2022a
ml CUDA/11.7.0

#cd /scratch/tmp/m_jost02/parallel_programming

code_path="/scratch/tmp/m_jost02/parallel_programming"
job="/Sequential"
output_path=/scratch/tmp/m_jost02/output/${dirname}
mkdir -p "$output_path"

buildname=build-${partition}

g++ -std=c++11 "${code_path}${job}/main.cpp" ${code_path}/Utils/matrix_init.cpp ${code_path}/Utils/general.cpp -o "${code_path}${job}/main"


for width in 512 1024 2048; do
    for iteration in {1..12}; do

        paramname="${width}-${width}-${device}"
        "${code_path}${job}/main" -x $width -y $width -z $width >> "${output_path}/${paramname}.out"
    done
done
