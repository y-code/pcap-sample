cmake_minimum_required( VERSION 3.22 )

set( CMAKE_CXX_STANDARD 20 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )

file( GLOB_RECURSE aggry_SRC ${CMAKE_CURRENT_LIST_DIR}/src/* )

message( "PROTO_HDRS: ${PROTO_HDRS}" )

set( RD_KAFKA_ROOT /usr/local/opt/librdkafka )

find_library( RD_KAFKA_LIBRARY NAMES rdkafka++ HINTS ${RD_KAFKA_ROOT} )
message( "Lib rdkafka: ${RD_KAFKA_LIBRARY}" )

add_executable( traffic-aggry )
target_include_directories( traffic-aggry
PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include
    ${CMAKE_BINARY_DIR}
    ${RD_KAFKA_ROOT}/include
)
target_sources( traffic-aggry
PRIVATE
    ${aggry_SRC}
)
target_link_libraries( traffic-aggry
PUBLIC
    ${Boost_LIBRARIES}
    libsniffy-domain
    ${RD_KAFKA_LIBRARY}
)

