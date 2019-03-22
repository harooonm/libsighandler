cmake_minimum_required(VERSION 2.8.9)
project(libsighandler)

set(CMAKE_CXX_FLAGS "-Wall -Wextra  -Wfatal-errors -D_POSIX_C_SOURCE=200112L -std=c11")

if(CMAKE_BUILD_TYPE EQUAL "Debug")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Og")
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
endif()

include_directories(include)
add_library(sighandler SHARED "src/sighandler.c")
set_property(TARGET sighandler PROPERTY POSITION_INDEPENDENT_CODE ON)
target_link_libraries(sighandler pthread)

#add_executable(test1  examples/test1.c)
#set_target_properties(test1 PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/examples)
#target_link_libraries(test1 pthread sighandler)
#target_include_directories(test1 PUBLIC ${CMAKE_SOURCE_DIR}/include)

#build examples
#file(GLOB esrcs examples/*.c)
#foreach (ex_src ${esrcs})
#	string(REPLACE ".c" "" ename ${ex_src})
#	get_filename_component(barename "${ename}" NAME)
#	add_executable(${barename}  ${ex_src})
#	set_target_properties(${barename} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/examples)
#	target_link_libraries(${barename} pthread sighandler)
#	target_include_directories(${barename} PUBLIC ${CMAKE_SOURCE_DIR}/include)
#endforeach(ex_src ${esrcs})
