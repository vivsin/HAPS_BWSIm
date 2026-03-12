PROJECT(LTESim)
cmake_minimum_required(VERSION 3.16)

# set(CMAKE_PREFIX_PATH "~/.arma/usr")
set(CMAKE_CXX_FLAGS "-std=c++11")

include_directories(${PROJECT_SOURCE_DIR})

SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/lib)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build)

MESSAGE(STATUS "Target Library Path" : ${LIBRARY_OUTPUT_PATH})

add_subdirectory(lib)
set(libList ${BWSIM_LIB_MODULES})


add_subdirectory(sls)
