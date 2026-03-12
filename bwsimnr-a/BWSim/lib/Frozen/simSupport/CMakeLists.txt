  set( simSupportLib "simSupport" )

file(GLOB fileLists src/*.cpp)
add_library(${simSupportLib} STATIC ${fileLists})
