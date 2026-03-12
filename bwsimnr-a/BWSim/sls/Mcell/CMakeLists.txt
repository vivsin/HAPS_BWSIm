
file(GLOB_RECURSE fileLists *.cpp)
set( MCell "MCell" )
add_library(${MCell} STATIC ${fileLists})

