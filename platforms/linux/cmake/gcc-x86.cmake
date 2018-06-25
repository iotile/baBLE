#### Toolchain file to build the project on x86 architecture ####

include("${CMAKE_CURRENT_LIST_DIR}/utils.cmake")

add_cached_flag(CMAKE_C_FLAGS "-m32")
add_cached_flag(CMAKE_CXX_FLAGS "-m32")
