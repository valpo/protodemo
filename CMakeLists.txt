cmake_minimum_required(VERSION 3.1.0)

# cmake -DCMAKE_PREFIX_PATH=/home/mwaack/Qt/5.6/gcc_64/lib/cmake ..

project(protodemo)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 14)

# check for protobuf
find_package(Protobuf REQUIRED)

# Find the QtWidgets library
find_package(Qt5Core REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5Widgets REQUIRED)

PROTOBUF_GENERATE_CPP(ProtoSources ProtoHeaders demo.proto)

add_executable(protodemo main.cpp protodemo.cpp protodemo.qrc protodemo.qrc protodemo.ui demo.proto ${ProtoSources})
target_link_libraries(protodemo ${PROTOBUF_LIBRARIES} Qt5::Core Qt5::Network Qt5::Widgets)

