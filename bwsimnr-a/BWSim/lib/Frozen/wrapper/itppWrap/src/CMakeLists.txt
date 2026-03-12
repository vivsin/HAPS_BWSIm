
file(GLOB fileLists *.cpp)
set( Wrapper "Wrapper" )
add_library(${Wrapper} STATIC ${fileLists})

