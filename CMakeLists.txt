cmake_minimum_required(VERSION 3.20) 
enable_testing()

project(sutils LANGUAGES CXX)

# set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_ERROR_DEPRECATED ON)
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

file(GLOB_RECURSE sutils_test_src CONFIGURE_DEPENDS
  string_utilities.hpp
  test/*.cpp
  test/*.hpp
)
add_executable(sutils_test
  "${sutils_test_src}"
)
target_include_directories(sutils_test
  PUBLIC "${CMAKE_SOURCE_DIR}"
)
# https://gitlab.kitware.com/cmake/cmake/-/issues/18837#note_722441
if ((MSVC) AND (MSVC_VERSION GREATER_EQUAL 1914))
	target_compile_options(sutils_test PUBLIC "/Zc:__cplusplus")
endif()

add_test(lib_test
  COMMAND sutils_test
)
