
file(GLOB_RECURSE fileLists1 *.cpp)

set( MCellLib "MCellLib" )
add_library(${MCellLib} STATIC ${fileLists1})

