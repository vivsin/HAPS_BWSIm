file(GLOB fileLists src/*.cpp)
  set( PRACH "PRACHLib" )


add_library(${PRACHLib} STATIC ${fileLists})
