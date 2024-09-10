cmake_minimum_required( VERSION 3.22 )

set( CMAKE_CXX_STANDARD 20 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )

file( GLOB_RECURSE src ${CMAKE_CURRENT_LIST_DIR}/src/* )

find_package( Boost 1.86.0 REQUIRED EXACT
REQUIRED COMPONENTS
	program_options
)

# set( RD_KAFKA_ROOT /usr/local/opt/librdkafka )

find_library( PCAP_LIBRARY NAMES pcap )
message( "Lib pcap: ${PCAP_LIBRARY}")

# find_library( RD_KAFKA_LIBRARY NAMES rdkafka++ HINTS ${RD_KAFKA_ROOT} )
# message( "Lib rdkafka: ${RD_KAFKA_LIBRARY}" )

add_library( libsniffy SHARED )
target_include_directories( libsniffy
PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include
    ${Protobuf_INCLUDE_DIRS}
    ${CMAKE_BINARY_DIR}
#    ${RD_KAFKA_ROOT}/include
)
target_sources( libsniffy PRIVATE ${src} )
target_link_libraries( libsniffy
PUBLIC
    libsniffy-domain
    ${PCAP_LIBRARY}
    ${RD_KAFKA_LIBRARY}
)

add_executable( sniffy )
target_sources( sniffy PRIVATE ${CMAKE_CURRENT_LIST_DIR}/main.cpp )
target_link_libraries( sniffy
PUBLIC
    libsniffy
    ${Boost_LIBRARIES}
)

set_target_properties( libsniffy PROPERTIES LINKER_LANGUAGE CXX )

