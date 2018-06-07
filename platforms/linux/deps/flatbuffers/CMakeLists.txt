cmake_minimum_required(VERSION 3.5)
project(flatbuffers)

add_library(flatbuffers INTERFACE)
target_include_directories(flatbuffers INTERFACE .)
