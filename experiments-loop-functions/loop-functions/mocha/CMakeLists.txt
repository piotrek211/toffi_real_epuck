# Set up ARGoS compilation information
include_directories(${CMAKE_SOURCE_DIR} ${ARGOS_INCLUDE_DIRS})
link_directories(${ARGOS_LIBRARY_DIRS})
link_directories(
	${CMAKE_BINARY_DIR}/src
    )

add_library(toffi_pu_loopfunc SHARED ToffiPuLoopFunc.h ToffiPuLoopFunc.cpp)
target_link_libraries(toffi_pu_loopfunc argos3plugin_${ARGOS_BUILD_FOR}_epuck)

add_library(toffi_ch_loopfunc SHARED ToffiChLoopFunc.h ToffiChLoopFunc.cpp)
target_link_libraries(toffi_ch_loopfunc argos3plugin_${ARGOS_BUILD_FOR}_epuck)

add_library(toffi_ps_loopfunc SHARED ToffiPsLoopFunc.h ToffiPsLoopFunc.cpp)
target_link_libraries(toffi_ps_loopfunc argos3plugin_${ARGOS_BUILD_FOR}_epuck)
