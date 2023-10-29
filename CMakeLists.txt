cmake_minimum_required( VERSION 3.22 )

set( CMAKE_CXX_STANDARD 20 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( protobuf_BUILD_TESTS OFF )
set( ABSL_BUILD_TESTING OFF )
set( ABSL_USE_GOOGLETEST_HEAD OFF )
set( ABSL_PROPAGATE_CXX_STD OFF )

file( GLOB_RECURSE src src/* )
file( GLOB_RECURSE proto proto/* )

project( y-pcap0
    VERSION 1.0
    LANGUAGES CXX
)

# find_package( absl REQUIRED )
find_package( Protobuf 3.21 REQUIRED )
# set(PROTOBUF_LIBRARY "protobuf::libprotobuf")

protobuf_generate_cpp( PROTO_SRCS PROTO_HDRS ${proto} )

add_library( ypcap0msg SHARED ${PROTO_SRCS} ${PROTO_HDRS} )
target_include_directories( ypcap0msg PUBLIC ${Protobuf_INCLUDE_DIRS} )
target_link_libraries( ypcap0msg
    ${Protobuf_LIBRARIES}
        #absl::algorithm
        #absl::base
        #absl::debugging
        #absl::flat_hash_map
        #absl::flags
        #absl::memory
        #absl::meta
        #absl::numeric
        #absl::random_random
        #sbsl::strings
        #absl::synchronization
        #absl::time
        #absl::utility
)

add_library( ypcap0 SHARED )
target_include_directories( ypcap0
    PUBLIC
        include
	${CMAKE_CURRENT_BINARY_DIR}
        /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/include
)
target_sources( ypcap0 PRIVATE ${src} )
target_link_libraries( ypcap0
    PUBLIC
        ypcap0msg
        ${Protobuf_LIBRARIES}
        libpcap.tbd
)

add_executable( main )
target_include_directories( main PUBLIC include )
target_sources( main PRIVATE main.cpp )
target_link_libraries( main
    PUBLIC
        ypcap0msg
        ${Protobuf_LIBRARIES}
        ypcap0
        libpcap.tbd
)

set_target_properties( ypcap0 PROPERTIES LINKER_LANGUAGE CXX )

