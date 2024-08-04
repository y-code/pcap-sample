cmake_minimum_required( VERSION 3.22 )

set( CMAKE_CXX_STANDARD 20 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )

file( GLOB_RECURSE proto ${CMAKE_CURRENT_LIST_DIR}/proto/* )

find_package( absl REQUIRED )
find_package( Protobuf 4.25.2 REQUIRED )

protobuf_generate_cpp( PROTO_SRCS PROTO_HDRS ${proto} )

add_library( libsniffy-domain SHARED )
target_include_directories( libsniffy-domain
PUBLIC
    ${Protobuf_INCLUDE_DIRS}
    ${PROTO_HDRS}
)
target_sources( libsniffy-domain
PRIVATE
    ${PROTO_SRCS}
)
target_link_libraries( libsniffy-domain
PUBLIC
    ${Protobuf_LIBRARIES}
    absl::log_internal_message
    absl::log_internal_check_op
)

