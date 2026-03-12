
file(GLOB_RECURSE fileLists *.cpp)
set( Link "Link" )
add_library(${Link} STATIC ${fileLists})

