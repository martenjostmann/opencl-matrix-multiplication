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

partition=gpu2080
implementation="native"
dirname=$(date +"%Y-%m-%dT%H-%M-%S-${partition}-${implementation}")

module purge
ml palma/2022a
ml CUDA/11.7.0

cd /scratch/tmp/m_jost02/parallel_programming

path=/scratch/tmp/m_jost02/test/${dirname}
mkdir -p "$path"

buildname=build-${partition}

(
cd native
rm -rf "$buildname"
mkdir "$buildname"
cd "$buildname"
g++ -std=c++11 "../Parallel/1. Baseline/main.cpp" ../Utils/matrix_init.cpp -lOpenCL -o main
)

for width in 1024, 2048; do
    paramname="${width}x${width}-g${gpu}"
    ./native/${buildname}/main -w $width >> "${path}/native.out"
done