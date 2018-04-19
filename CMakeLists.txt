# CMakeFile.txt
# for wlibTools


####################
# Project Settings
####################
cmake_minimum_required(VERSION 3.0.0)

project(wlibCameraTools)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories(src/)
file(GLOB_RECURSE jubeon_cpp_files src/*.cpp)
file(GLOB_RECURSE jubeon_header_files src/*.hpp)

set(LIBRARY_NAME "wlibTools")

set(MAYA_ROOT_PATH "$ENV{MAYA2015_PATH}" CACHE PATH "Maya2015のインストール先フォルダを指定してください")


set(VS_COMPILE_FLAGS "-GR")
set(VS_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -export:initializePlugin -export:uninitializePlugin")

add_definitions(-DWIN32 -D_WIN64 -D_WINDOWS -D_USRDLL -DNT_PLUGIN -DREQUIRE_IOSTREAM)
add_definitions(${VS_COMPILE_FLAGS})
set(CMAKE_CXX_FLAGS  ${CMAKE_CXX_FLAGS} ${VS_COMPILE_FLAGS} )
set(CMAKE_SHARED_LINKER_FLAGS ${VS_LINKER_FLAGS})


####################
# Include Directory
####################
set(INCLUDE_DIR
"src"
${MAYA_ROOT_PATH}/include
)
include_directories("${INCLUDE_DIR}")

set(LIBRARY_DIR
  ${MAYA_ROOT_PATH}/lib
)
link_directories("${LIBRARY_DIR}")

add_library(${LIBRARY_NAME} SHARED ${jubeon_cpp_files} ${jubeon_header_files})
set_target_properties(${LIBRARY_NAME} PROPERTIES SUFFIX ".mll")

target_link_libraries(${LIBRARY_NAME}
  OpenMaya.lib
  Foundation.lib
  OpenMaya.lib
  OpenMayaUI.lib
  OpenMayaRender.lib
  OpenMayaAnim.lib
)

source_group("main" REGULAR_EXPRESSION "src/(.*).(cpp|hpp)")
source_group("command" REGULAR_EXPRESSION "src/command/(.*).(cpp|hpp)")
source_group("exception" REGULAR_EXPRESSION "src/exception/(.*).(cpp|hpp)")
source_group("exception\\MStatusException" REGULAR_EXPRESSION "src/exception/MStatusException(.*).(cpp|hpp)")
source_group("node" REGULAR_EXPRESSION "src/node/(.*).(cpp|hpp)")
source_group("translator" REGULAR_EXPRESSION "src/translator/(.*).(cpp|hpp)")
source_group("utility" REGULAR_EXPRESSION "src/utility/(.*).(cpp|hpp)")

