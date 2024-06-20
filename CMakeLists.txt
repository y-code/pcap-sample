cmake_minimum_required( VERSION 3.22 )

set( CMAKE_CXX_STANDARD 20 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )

file( GLOB_RECURSE src src/* )
file( GLOB_RECURSE proto proto/* )

project( y-pcap0
    VERSION 1.0
    LANGUAGES CXX
)

find_package( absl REQUIRED )
find_package( Protobuf 4.25.2 REQUIRED )
find_package( Boost 1.84.0 REQUIRED )

protobuf_generate_cpp( PROTO_SRCS PROTO_HDRS ${proto} )

add_library( ypcap0msg SHARED ${PROTO_SRCS} ${PROTO_HDRS} )
target_include_directories( ypcap0msg
    PUBLIC
        ${Protobuf_INCLUDE_DIRS}
        ${Boost_INCLUDE_DIRS}
)
target_link_libraries( ypcap0msg
    PUBLIC
        ${Protobuf_LIBRARIES}
        absl::log_internal_message
        absl::log_internal_check_op
        ${Boost_LIBRARIES}
)

find_library( PCAP_LIBRARY NAMES pcap )
message(PCAP_LIBRARY="${PCAP_LIBRARY}")

add_library( ypcap0 SHARED )
target_include_directories( ypcap0
    PUBLIC
        include
        ${Protobuf_INCLUDE_DIRS}
        ${CMAKE_CURRENT_BINARY_DIR}
)
target_sources( ypcap0 PRIVATE ${src} )
target_link_libraries( ypcap0
    PUBLIC
        ypcap0msg
        ${PCAP_LIBRARY}
)

add_executable( main )
target_include_directories( main PUBLIC include )
target_sources( main PRIVATE main.cpp )
target_link_libraries( main
    PUBLIC
        ypcap0
)

set_target_properties( ypcap0 PROPERTIES LINKER_LANGUAGE CXX )

