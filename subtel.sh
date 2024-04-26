#!/bin/bash
#JSUB -q normal
#JSUB -n 80
#JSUB -e error.log
#JSUB -o output.%J.log
#JSUB -J my_job.%J
##JSUB -R span[hosts=1]

module purge
module load glibc/2.39.0 gcc/12.1.0 cmake/3.29.0 ninja/commit_3037 compiler/2021.3.0 mkl/2021.3.0

dirPath="/home/20009100240/vertices"

which g++
which icpc
uname -a
free -m
nproc

CC=icc CXX=icpc cmake -DCMAKE_BUILD_TYPE=Release "-DCMAKE_MAKE_PROGRAM=ninja" -G Ninja -S $dirPath -B $dirPath/cmake-build-release
cmake --build $dirPath/cmake-build-release --target vertices -j 80

timestamp=$(date +'%Y%m%d%H%M%S')
mv $dirPath/cmake-build-release/vertices $dirPath/cmake-build-release/vertices$timestamp
$dirPath/cmake-build-release/vertices$timestamp