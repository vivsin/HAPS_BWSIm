
file(GLOB_RECURSE fileLists1 *.cpp)
  set( channelCoderLib "channelCoder" )
add_library(${channelCoderLib} STATIC ${fileLists1})

