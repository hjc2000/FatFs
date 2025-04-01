set(ProjectName FatFs)
project(${ProjectName})
enable_language(ASM)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 99)

add_library(${ProjectName} STATIC)
target_import_src(${ProjectName})
