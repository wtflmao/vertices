cmake_minimum_required(VERSION 3.27)
project(vertices)

set(CMAKE_CXX_STANDARD 23)

add_executable(vertices main.cpp
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
        src/io/readObj.cpp
        src/io/readObj.h
        src/io/readFace.cpp
        src/io/readFace.h
        src/io/readVertex.cpp
        src/io/readVertex.h
        src/io/readTexture.cpp
        src/io/readTexture.h
        src/io/readers.h
        src/common.h
)
