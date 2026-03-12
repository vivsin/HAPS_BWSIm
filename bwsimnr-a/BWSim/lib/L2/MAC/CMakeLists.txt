
file(GLOB_RECURSE fileLists *.cpp)
set( MAC "MAC" )
add_library(${MAC} STATIC ${fileLists})

