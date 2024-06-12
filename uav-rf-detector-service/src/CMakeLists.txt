add_executable(${USER_PROJECT_NAME})

find_package(Boost REQUIRED)
find_package(absl CONFIG REQUIRED)
find_package(protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)
find_package(asio-grpc REQUIRED)

file(GLOB_RECURSE HEADERS
        ${CMAKE_CURRENT_SOURCE_DIR}/*.h
        ${PROJECT_SOURCE_DIR}/extern/include/*.hh
)

file(GLOB_RECURSE SOURCES
        #${include_dir}/*.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

file(GLOB_RECURSE protos_sources
        ${PROJECT_SOURCE_DIR}/proto/*.hpp
        ${PROJECT_SOURCE_DIR}/proto/*.cpp
)

file(GLOB_RECURSE protos
        ${PROJECT_SOURCE_DIR}/files/proto/address.proto
        ${PROJECT_SOURCE_DIR}/files/proto/addressbook.proto
        ${PROJECT_SOURCE_DIR}/files/proto/helloworld.proto
        ${PROJECT_SOURCE_DIR}/files/proto/rfdetector.proto
)

target_sources(${USER_PROJECT_NAME}
        PRIVATE
        ${HEADERS}
        ${SOURCES}
        ${protos_sources}
        ${protos}
)

target_include_directories(${USER_PROJECT_NAME}
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/lib
        ${CMAKE_CURRENT_BINARY_DIR} # protobuf generated files
        ${PROJECT_SOURCE_DIR}/src
        ${PROJECT_SOURCE_DIR}/extern/include
)

include_directories(${Boost_INCLUDE_DIRS})
add_definitions(-DBOOST_ASIO_HAS_CO_AWAIT)

target_link_libraries(${USER_PROJECT_NAME}
        PRIVATE
            Boost::boost
            protobuf::libprotobuf
            gRPC::grpc
            gRPC::grpc++
            asio-grpc::asio-grpc
            Boost::headers
)
#target_link_libraries(${USER_PROJECT_NAME}
#        PUBLIC
#            libcuckoo
#)

get_target_property(grpc_cpp_plugin_location gRPC::grpc_cpp_plugin LOCATION)
protobuf_generate(TARGET ${USER_PROJECT_NAME} APPEND_PATH ON ${PROJECT_SOURCE_DIR}/files/proto)
protobuf_generate(TARGET ${USER_PROJECT_NAME} LANGUAGE grpc GENERATE_EXTENSIONS .grpc.pb.h .grpc.pb.cc
                  PLUGIN "protoc-gen-grpc=${grpc_cpp_plugin_location}"
                  APPEND_PATH ON ${PROJECT_SOURCE_DIR}/files/proto)