add_executable(${USER_PROJECT_NAME})

find_package(Boost 1.82.0 REQUIRED COMPONENTS)

if(NOT Boost_FOUND)
    message(FATAL_ERROR "Could not find boost!")
endif()
message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
message(STATUS "Boost_LIBRARIES: ${Boost_LIBRARIES}")
message(STATUS "Boost_VERSION: ${Boost_VERSION}")

find_package(absl CONFIG REQUIRED)
find_package(protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)
find_package(asio-grpc REQUIRED)

find_package(Qt5 COMPONENTS Qml            REQUIRED)
find_package(Qt5 COMPONENTS Core           REQUIRED)
find_package(Qt5 COMPONENTS Quick          REQUIRED)
find_package(Qt5 COMPONENTS Network        REQUIRED)
find_package(Qt5 COMPONENTS Location       REQUIRED)
find_package(Qt5 COMPONENTS WebSockets     REQUIRED)
find_package(Qt5 COMPONENTS QuickControls2 REQUIRED)
find_package(Qt5 COMPONENTS Multimedia     REQUIRED)
find_package(Qt5 COMPONENTS Widgets        REQUIRED)

find_package(Qt5QmlImportScanner REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(HEADERS
        # none
)

file(GLOB_RECURSE SOURCES
        #${include_dir}/*.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

file(GLOB_RECURSE protos_sources
        ${PROJECT_SOURCE_DIR}/proto/*.hpp
        ${PROJECT_SOURCE_DIR}/proto/*.cpp
)

file(GLOB_RECURSE protos
        ${PROJECT_SOURCE_DIR}/files/proto/address.proto
        ${PROJECT_SOURCE_DIR}/files/proto/addressbook.proto
        ${PROJECT_SOURCE_DIR}/files/proto/helloworld.proto
)

set(QRC_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/assets/qml.qrc
)

qt5_add_resources(RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/assets/qml.qrc)

target_sources(${USER_PROJECT_NAME}
        PRIVATE
        ${HEADERS}
        ${SOURCES}
        ${RESOURCES}
        ${protos_sources}
        ${protos}
)

target_include_directories(${USER_PROJECT_NAME}
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR} # protobuf generated files
        ${PROJECT_SOURCE_DIR}/src
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
        Qt5::Core
        Qt5::Quick
        Qt5::Location
        Qt5::Qml
        Qt5::QuickControls2
        Qt5::Multimedia
        Qt5::Widgets
)

qt5_import_qml_plugins(${USER_PROJECT_NAME})

get_target_property(grpc_cpp_plugin_location gRPC::grpc_cpp_plugin LOCATION)
protobuf_generate(TARGET ${USER_PROJECT_NAME} APPEND_PATH ON ${PROJECT_SOURCE_DIR}/files/proto)
protobuf_generate(TARGET ${USER_PROJECT_NAME} LANGUAGE grpc GENERATE_EXTENSIONS .grpc.pb.h .grpc.pb.cc PLUGIN "protoc-gen-grpc=${grpc_cpp_plugin_location}"
        APPEND_PATH ON ${PROJECT_SOURCE_DIR}/files/proto)