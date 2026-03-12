file(GLOB fileLists src/*.cpp)
  set( PHYLib "PHYLib" )


add_library(${PHYLib} STATIC ${fileLists})
