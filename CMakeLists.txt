cmake_minimum_required(VERSION 3.26.0)
project(Juliet VERSION 0.1.0 LANGUAGES C CXX CUDA)

set(FullOutputDir "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${FullOutputDir}/static-libs")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${FullOutputDir}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${FullOutputDir}")

set(SrcPath "${CMAKE_SOURCE_DIR}/src/sources")
set(HeaderPath "${CMAKE_SOURCE_DIR}/src/include")
file(GLOB_RECURSE Sources CONFIGURE_DEPENDS "${SrcPath}/*.cpp" "${SrcPath}/*.c" "${SrcPath}/*.cu")

include_directories(${HeaderPath} $ENV{OPENGL_PATH})

find_package(Threads REQUIRED)

add_executable(${PROJECT_NAME} ${Sources})
# set_property(TARGET ${PROJECT_NAME} PROPERTY CUDA_SEPARABLE_COMPILATION ON)
set_target_properties(${PROJECT_NAME} PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
target_compile_options(${PROJECT_NAME} PRIVATE -O3)
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)

