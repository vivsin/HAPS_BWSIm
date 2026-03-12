
cmake_minimum_required(VERSION 2.6)

FILE(GLOB headerfiles "*.h")
INSTALL(FILES ${headerfiles} DESTINATION include/simSupport)
