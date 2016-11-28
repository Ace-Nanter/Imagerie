# Project #-----------------------------------------------------------------------------------------
project ( imagerie )

cmake_minimum_required ( VERSION 2.8.9 )

find_package(X11 REQUIRED)
find_package(Threads REQUIRED)

# C++ Warning Level #-------------------------------------------------------------------------------
if ( CMAKE_COMPILER_IS_GNUCXX )
 set ( CMAKE_CXX_FLAGS "-Wall -pedantic ${CMAKE_CXX_FLAGS}" )
endif()

# C++11 #-------------------------------------------------------------------------------------------
include ( CheckCXXCompilerFlag )

check_cxx_compiler_flag ( "-std=gnu++11" COMPILER_SUPPORTS_CPP11 )
check_cxx_compiler_flag ( "-std=gnu++0x" COMPILER_SUPPORTS_CPP0X )

if ( COMPILER_SUPPORTS_CPP11 )
 set ( CMAKE_CXX_FLAGS "-std=gnu++11 ${CMAKE_CXX_FLAGS}" )
elseif( COMPILER_SUPPORTS_CPP0X )
 set ( CMAKE_CXX_FLAGS "-std=gnu++0x ${CMAKE_CXX_FLAGS}" )
else ()
 message ( STATUS "Compiler ${CMAKE_CXX_COMPILER} has no C++11 support." )
endif ()

message ( STATUS "Compiler flags: ${CMAKE_CXX_FLAGS}" )

# Sources #-----------------------------------------------------------------------------------------
set ( 	HEADERS
    )

set ( SOURCES
    )

include_directories (src/ lib/)

# Executables #-------------------------------------------------------------------------------------
add_executable ( ${CMAKE_PROJECT_NAME}
                 src/main.cpp
                 ${HEADERS}
                 ${SOURCES}
               )

# Build #-------------------------------------------------------------------------------------------
set_target_properties ( ${CMAKE_PROJECT_NAME} PROPERTIES LINKER_LANGUAGE C )
target_link_libraries ( ${CMAKE_PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT} ${X11_LIBRARIES})
