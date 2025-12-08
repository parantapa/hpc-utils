cmake_minimum_required(VERSION 4.0)

project(
    hpc-utils
    VERSION 0.1
    LANGUAGES CXX)

option(BUILD_TESTING "Build tests" OFF)

find_package(OpenMP REQUIRED)
find_package(fmt REQUIRED)
find_package(phmap REQUIRED)

find_package(random123 REQUIRED)
find_package(Arrow REQUIRED)
find_package(HDF5 REQUIRED)

add_library(hpc-utils src/allocator.cpp)
target_sources(hpc-utils
PUBLIC
    FILE_SET HEADERS
    BASE_DIRS include
    FILES
        include/hpc_utils/panic.hpp
        include/hpc_utils/allocator.hpp
        include/hpc_utils/static_array.hpp
        include/hpc_utils/containers.hpp

	include/hpc_utils/rng_utils.hpp
	include/hpc_utils/hdf5_utils.hpp
	include/hpc_utils/arrow_utils.hpp
)
target_link_libraries(hpc-utils
PUBLIC
    OpenMP::OpenMP_CXX
    fmt::fmt
    phmap

    random123::random123
    arrow::arrow
    hdf5::hdf5
    hdf5::hdf5_cpp
)
target_compile_features(hpc-utils
PUBLIC
    cxx_std_23
)


# if (BUILD_TESTING)
#
#     add_executable(hello test/hello.cpp)
#     target_link_libraries(hello
#     PRIVATE
#         hpc-utils
#     )
#
# endif()

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/hpc-utils-version.cmake"
    COMPATIBILITY ExactVersion
)

install(
    TARGETS hpc-utils
    EXPORT hpc-utils-targets
    FILE_SET HEADERS
)

install(
    FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/hpc-utils-config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/hpc-utils-version.cmake"
    DESTINATION
        "${CMAKE_INSTALL_LIBDIR}/cmake/hpc-utils"
)

install(
    EXPORT hpc-utils-targets
    DESTINATION
        "${CMAKE_INSTALL_LIBDIR}/cmake/hpc-utils"
    NAMESPACE hpc-utils::
    FILE hpc-utils-targets.cmake
)
