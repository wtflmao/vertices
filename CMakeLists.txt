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
        src/io/readLines.cpp
        src/io/readLines.h
        src/io/readFace.cpp
        src/io/readFace.h
        src/io/readVertex.cpp
        src/io/readVertex.h
        src/io/readTexture.cpp
        src/io/readTexture.h
        src/io/readNewItem.h
        src/common.h
        src/io/readMtl.cpp
        src/io/readMtl.h
        src/io/readNewItem.cpp
        main.h
        src/structures/box.cpp
        src/structures/box.h
        src/structures/field.cpp
        src/structures/field.h
)
