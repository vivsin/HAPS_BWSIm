
file(GLOB_RECURSE fileLists *.cpp)
set( MPISupportLib "MPISupport" )
add_library(${MPISupportLib} STATIC ${fileLists})

