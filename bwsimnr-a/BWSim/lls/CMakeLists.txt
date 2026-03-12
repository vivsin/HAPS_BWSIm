
include(../lib/libListConfig.txt)


if(${USING_MPI} MATCHES "true" )

    find_package(MPI REQUIRED)
    set(CMAKE_CXX_COMPILE_FLAGS ${CMAKE_CXX_COMPILE_FLAGS} ${MPI_COMPILE_FLAGS})
    set(CMAKE_CXX_LINK_FLAGS ${CMAKE_CXX_LINK_FLAGS} ${MPI_LINK_FLAGS})

    include_directories(${MPI_INCLUDE_PATH})

    MESSAGE(STATUS "Found MPI " "${MPI_INCLUDE_PATH}")

endif(${USING_MPI} MATCHES "true" )

add_subdirectory(src/)
set(libList llssrc ${libList})

add_subdirectory(apps/TDLCDL/)
