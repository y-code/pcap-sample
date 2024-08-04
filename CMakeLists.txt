cmake_minimum_required( VERSION 3.22 )

set( CMAKE_CXX_STANDARD 20 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )

project( y-pcap
    VERSION 1.0
    LANGUAGES CXX
)

message( "CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}" )

include( ${CMAKE_CURRENT_SOURCE_DIR}/sniffy-domain/CMakeLists.txt )
include( ${CMAKE_CURRENT_SOURCE_DIR}/sniffy/CMakeLists.txt )
include( ${CMAKE_CURRENT_SOURCE_DIR}/traffic-aggry/CMakeLists.txt )

