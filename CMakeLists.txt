#
# Taken from gtest example, modified
#
#####################################

set(CMAKE_BUILD_TYPE Debug)

cmake_minimum_required(VERSION 3.14)
project(a2)

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include (${CMAKE_ROOT}/Modules/FindX11.cmake)
include_directories(${X11_INCLUDE_DIR})
include (ExternalProject)
include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG v1.x
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)


#needed for easygl
add_compile_definitions(PLATFORM=X11)
add_compile_options(-Wno-deprecated-declarations)

# gprof
#add_compile_options(-pg)

FetchContent_GetProperties(spdlog)
if (NOT spdlog_POPULATED)
    FetchContent_Populate(spdlog)
    add_subdirectory(${spdlog_SOURCE_DIR} ${spdlog_BINARY_DIR})
endif()

if(NOT TARGET spdlog)
  find_package(spdlog REQUIRED)
endif()

# Add git commit history to build output
execute_process(COMMAND
  git describe --match=NeVeRmAtCh --always --abbrev=40 --dirty
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
  OUTPUT_VARIABLE GIT_COMMIT
  ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)

add_compile_definitions(GIT_COMMIT="${GIT_COMMIT}")

enable_testing()

#
# TEST SUITES
##################

add_executable(
  unit_tests
  circuit.cpp
  partition.cpp
  file_read_test.cc
  partition_test.cc
)

#
# ACTUAL PROGRAM
##################
add_executable(
  a3
  main.cpp
  ui.cpp
  circuit.cpp
  partition.cpp
  easygl/graphics.cpp
)

include_directories(${SOURCE_DIR}/include)

# there is definitely a better way to handle this
# but I am not going to sort that out tonight
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	set(LIBEXT dylib)
else()
	set(LIBEXT so)
endif()

target_link_libraries(
  a3
  spdlog::spdlog
  ${X11_LIBRARIES}
)
target_link_libraries(
  unit_tests
  GTest::gtest_main
  spdlog::spdlog
)
target_compile_definitions(
  unit_tests
  PUBLIC GTEST
)

include(GoogleTest)
gtest_discover_tests(unit_tests)
