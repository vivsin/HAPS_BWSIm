  set( sequenceLib "sequence" )

file(GLOB fileLists src/*.cpp)
add_library(${sequenceLib} STATIC ${fileLists})
