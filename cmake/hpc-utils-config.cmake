include(CMakeFindDependencyMacro)

find_dependency(OpenMP)
find_dependency(fmt)
find_dependency(phmap)

include("${CMAKE_CURRENT_LIST_DIR}/hpc-utils-targets.cmake")
