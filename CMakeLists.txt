cmake_minimum_required(VERSION 4.0)

project(
    hpc-utils
    VERSION 0.1
    LANGUAGES CXX)

option(BUILD_TESTING "Build tests" OFF)

find_package(fmt REQUIRED)

add_library(hpc-utils src/say_hello.cpp)
target_sources(hpc-utils
PUBLIC
    FILE_SET HEADERS
    BASE_DIRS include
    FILES
        include/hpc-utils/say_hello.hpp
)
target_link_libraries(hpc-utils
PUBLIC
    fmt::fmt
)
target_compile_features(hpc-utils
PUBLIC
    cxx_std_23
)


if (BUILD_TESTING)

    add_executable(hello test/hello.cpp)
    target_link_libraries(hello
    PRIVATE
        hpc-utils
    )

endif()

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
