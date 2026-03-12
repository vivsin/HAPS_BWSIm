


include(libListConfig.txt)
configure_file(${PROJECT_SOURCE_DIR}/lib/libListConfig.h.in ${PROJECT_SOURCE_DIR}/lib/libListConfig.h)

add_subdirectory(Frozen/mcell)
add_subdirectory(Frozen/simSupport)
add_subdirectory(Frozen/channelCoder)
add_subdirectory(Frozen/sequence)
add_subdirectory(Frozen/Threadpool)
add_subdirectory(L1)
add_subdirectory(L2/MAC)

 
set( MCellLib "MCellLib" )
set( PDCCHLib "PDCCHLib") 
set( SCHLib "SCHLib") 
set( PHYLib "PHYLib") 
set( SSBLib "SSBLib") 
set( SRSLib "SRSLib") 
set( CSIRSLib "CSIRSLib") 
set( channelCoderLib "channelCoder" )
set( sequence "sequence" )
set( simSupportLib "simSupport" )
set( Wrapper "Wrapper" )
set (MAC "MAC")
set (Thread "Thread")


if( ${USING_ITPP} MATCHES "true" )

find_library(ITPP itpp)
add_subdirectory(${CMAKE_SOURCE_DIR}/lib/Frozen/wrapper/itppWrap/src)
set(extLib ${Wrapper} ${ITPP})

elseif( ${USING_ITPP} MATCHES "false" )

find_package(Armadillo REQUIRED)
include_directories(${ARMADILLO_INCLUDE_DIRS})
add_subdirectory(${CMAKE_SOURCE_DIR}/lib/Frozen/wrapper/armaWrap/src)
set(extLib ${Wrapper} ${ARMADILLO_LIBRARIES})

endif( ${USING_ITPP} MATCHES "true" )

if( ${USING_SECURITY} MATCHES "true" )

add_subdirectory(Frozen/Security)
set(securityLib Security)

endif( ${USING_SECURITY} MATCHES "true" )


set(BWSIM_LIB_MODULES ${MAC} ${SCHLib} ${PDCCHLib} ${CSIRSLib} ${PHYLib} ${SSBLib} ${SRSLib} ${sequence} ${MCellLib} ${channelCoderLib} ${simSupportLib} ${extLib} ${Thread} ${securityLib})


# Updates the value of libList in the parent scope
set(BWSIM_LIB_MODULES ${BWSIM_LIB_MODULES} PARENT_SCOPE)	 

