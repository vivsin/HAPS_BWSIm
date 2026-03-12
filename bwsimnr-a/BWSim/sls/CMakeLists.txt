
include(../lib/libListConfig.txt)


if(${USING_MPI} MATCHES "true")

    find_package(MPI REQUIRED)
    set(CMAKE_CXX_COMPILE_FLAGS ${CMAKE_CXX_COMPILE_FLAGS} ${MPI_COMPILE_FLAGS})
    set(CMAKE_CXX_LINK_FLAGS ${CMAKE_CXX_LINK_FLAGS} ${MPI_LINK_FLAGS})

    include_directories(${MPI_INCLUDE_PATH})

    MESSAGE(STATUS "Found MPI " "${MPI_INCLUDE_PATH}")

    add_subdirectory(MPI)
    set(libList MPISupport ${MPI_LIBRARIES} ${libList})

endif(${USING_MPI} MATCHES "true")

add_subdirectory(Mcell)
set(libList MCell ${libList})

add_subdirectory(Link)
set(libList Link ${libList})

add_subdirectory(sim/src)
set(libList simLib ${libList})


add_subdirectory(apps/lite/)
