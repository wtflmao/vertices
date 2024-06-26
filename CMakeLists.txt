cmake_minimum_required(VERSION 3.27)
project(vertices)

# remember to edit src/compatibility.h if you want to change the C++ standard
set(CMAKE_CXX_STANDARD 17)

add_executable(vertices src/main.cpp
        src/structures/ray.cpp
        src/structures/ray.h
        src/structures/triangle.cpp
        src/structures/triangle.h
        src/structures/point.cpp
        src/structures/point.h
        src/structures/vec.cpp
        src/structures/vec.h
        src/structures/item.cpp
        src/structures/item.h
        src/io/readLines.cpp
        src/io/readLines.h
        src/io/readFace.cpp
        src/io/readFace.h
        src/io/readVertex.cpp
        src/io/readVertex.h
        src/io/readTexture.cpp
        src/io/readTexture.h
        src/io/readNewItem.h
        src/io/readMtl.h
        src/common.h
        src/io/readMtl.cpp
        src/io/readMtl.h
        src/io/readNewItem.cpp
        src/main.h
        src/structures/box.cpp
        src/structures/box.h
        src/structures/field.cpp
        src/structures/field.h
        src/structures/node.cpp
        src/structures/node.h
        src/structures/camera.cpp
        src/structures/camera.h
        src/structures/pixel.cpp
        src/structures/pixel.h
        #src/basic/coordTransform.cpp
        #src/basic/coordTransform.h
        #lib/transform3d/transforms3d.h
        #lib/transform3d/trans_forms.cpp
        #lib/transform3d/trans_forms_group.cpp
        #lib/eigen3/Eigen
        src/basic/BRDF.cpp
        src/basic/BRDF.h
        src/logger/loggingFacility.h
        src/logger/stdoutLogger.cpp
        src/logger/stdoutLogger.h
        src/logger/coutLogger.cpp
        src/logger/coutLogger.h
        src/logger/filesystemLogger.cpp
        src/logger/filesystemLogger.h
        src/structures/imagePlane.cpp
        src/structures/imagePlane.h
        lib/thread_pool/thread_pool.h
        src/basic/threadPool.h
        src/basic/functionWrapper.h
        src/basic/threadSafeQueue.h
        lib/BitmapPlusPlus/BitmapPlusPlus.hpp
        src/visualizer/toBitmap.cpp
        src/visualizer/toBitmap.h
        src/compatibility.h
        src/io/infoAppender.cpp
        src/io/infoAppender.h
        lib/JSON/json.hpp
        lib/JSON/json_fwd.hpp
        src/io/readNormal.cpp
        src/io/readNormal.h
        src/structures/mtlDataset.h
)

if (UNIX AND NOT APPLE)
    find_package(Threads REQUIRED)
    target_link_libraries(vertices PRIVATE Threads::Threads)
endif ()

find_package(OpenMP)
if (OpenMP_CXX_FOUND)
    target_link_libraries(vertices PUBLIC OpenMP::OpenMP_CXX)
endif ()

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    # GCC  AVX512
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx512f -mavx512vl -mavx512bw -mavx512dq -mavx512ifma -mavx512vbmi2 -mprefer-vector-width=512")
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx2 -mprefer-vector-width=256")
    # GCC  vectorization with AVX512
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    # GCC  vectorization on
    ##set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ftree-vectorize -march=native -mtune=native -fstrict-aliasing -fopenmp-simd")
    # GCC turn off sth
    ##set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-trapping-math -fno-math-errno")
    # GCC vectorization report verbose
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopt-info-vec-optimized -fopt-info-loop-optimized")
    # gcc more warnings
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Wno-unused-parameter")

elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -xCOMMON-AVX2")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -qopt-zmm-usage=high -fp-model fast=2")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fp-trap-all=none -parallel")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ansi-alias -qopenmp-simd")
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -qopt-report=5 -qopt-report-phase=loop,vec,openmp")

elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    # it's always enabled on MSVC

elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fstrict-aliasing -march=native -mtune=native -fopenmp-simd")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvectorize -fno-math-errno")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx256f")
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Rpass=loop-vectorize -Rpass-missed=loop-vectorize -Rpass-analysis=loop-vectorize")
endif ()
