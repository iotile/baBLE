#### Toolchain file to build the project on x86 architecture ####

include("${CMAKE_CURRENT_LIST_DIR}/utils.cmake")

set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)

add_cached_flag(CMAKE_C_FLAGS "-m32")
add_cached_flag(CMAKE_CXX_FLAGS "-m32")
