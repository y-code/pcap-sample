cmake_minimum_required( VERSION 3.22 )

set( CMAKE_CXX_STANDARD 20 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )

file( GLOB_RECURSE aggry_SRC ${CMAKE_CURRENT_LIST_DIR}/src/* )

message( "PROTO_HDRS: ${PROTO_HDRS}" )

add_executable( traffic-aggry )
target_include_directories( traffic-aggry
PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include
    ${CMAKE_BINARY_DIR}
)
target_sources( traffic-aggry
PRIVATE
    ${aggry_SRC}
)
target_link_libraries( traffic-aggry
PUBLIC
    ${Boost_LIBRARIES}
    libsniffy-domain
)

