file(GLOB fileLists src/*.cpp)
  set( PDCCHLib "PDCCHLib" )


add_library(${PDCCHLib} STATIC ${fileLists})
