
add_subdirectory(events/src)
set(libList eventLib ${libList})

set(main SLSMain)
add_executable(${main}.run ${main}.cpp)

message(STATUS "Libraries Available : " "${libList}")

target_link_libraries(${main}.run ${libList} )

execute_process(COMMAND git describe --abbrev=12 --tags OUTPUT_VARIABLE _GIT_BUILD_ ERROR_QUIET)
add_definitions(-DGITVERSION=${_GIT_BUILD_})
message(STATUS "Setting Build Version " ${_GIT_BUILD})
