

set(main DLThrpt)

add_executable(${main}.run ${main}.cpp ${UEcontext})
message(STATUS "Libraries Available : " "${libList}")
find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

target_link_libraries(${main}.run ${libList})

execute_process(COMMAND git describe --abbrev=12 --tags OUTPUT_VARIABLE _GIT_BUILD_ ERROR_QUIET)
add_definitions(-DGITVERSION=${_GIT_BUILD_})
message(STATUS "Setting Build Version " ${_GIT_BUILD})




# cmake_minimum_required (VERSION 3.5)
# project(Scheduler)
# set (CMAKE_CXX_STANDARD 11)
# message(STATUS "Libraries Available : " "${libList}")

# set(libList ${libList})

# message(STATUS "Compiling : ${PROJECT_NAME}")
# set (CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR})
# message(STATUS "Cmake binary directory : ${CMAKE_SOURCE_DIR}")
# set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
# message(STATUS "Cmake executable directory : ${CMAKE_BINARY_DIR}")
# include_directories(${PROJECT_SOURCE_DIR})


# include_directories(../../../lib/L2/MAC/include/Application)
# include_directories(../../../lib/L2/MAC/include/Others)
# include_directories(../../../lib/Frozen/wrapper/itppWrap)
# include_directories(../../../lib/Frozen/simSupport/include)


# file(GLOB Scheduler SchedulerCode/src/*.cpp)
# file(GLOB UEcontext UEContext/src/*.cpp)


# file(GLOB TrafficGenApp ../../../lib/L2/MAC/src/Application/*.cpp)
# file(GLOB TrafficGenOthers ../../../lib/L2/MAC/src/Others/*.cpp)
# file(GLOB ITPPfiles ../../../lib/Frozen/wrapper/itppWrap/src/*.cpp)
# file(GLOB SimSupportfiles ../../../lib/Frozen/simSupport/src/*.cpp)

# # file(GLOB SimSupport TrafficGen_WPF/lib/simSupport/src/*.cpp)
# # file(GLOB libWrapper TrafficGen_WPF/lib/wrapper/itppWrap/src/*.cpp)
# #file(GLOB ITPPWRAP itppWrap/src/*.cpp)

# find_library(ITPP itpp)
# set(libList ${libList} ${ITPP})
# add_executable(${PROJECT_NAME}
# main1.cpp
#  ${Scheduler}
#  ${TrafficGenApp}
#  ${TrafficGenOthers}
#  #${Others}
#  ${UEcontext}
#  #${SimSupport}
#  ${ITPPfiles}
#  ${SimSupportfiles}
#  #${libListQAS}
# )

# #set(BASIC_LIBRARIES "-lboost_system -lboost_thread -lpthread -lboost_date_time -lcurl")
# set(BASIC_LIBRARIES "-lboost_system -lboost_thread -lpthread -lboost_date_time")

# find_package(OpenMP)
# if (OPENMP_FOUND)
#     set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
#     set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
#     set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
# endif()

# target_link_libraries(${PROJECT_NAME} ${libList} ${BASIC_LIBRARIES})
