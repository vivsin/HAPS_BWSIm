file(GLOB fileLists src/*.cpp)
  set( SSBLib "SSBLib" )


add_library(${SSBLib} STATIC ${fileLists})
