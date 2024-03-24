module purge
module add glibc/2.39.0 cmake/3.29.0 ninja/commit_3037 gcc/12.1.0

cmake -DCMAKE_BUILD_TYPE=Release "-DCMAKE_MAKE_PROGRAM=ninja" -G Ninja -S ~/vertices -B ~/vertices/cmake-build-release
cmake --build ~/vertices/cmake-build-release --target vertices -j 6 && ~/vertices/cmake-build-release/vertices