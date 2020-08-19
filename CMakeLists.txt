cmake_minimum_required(VERSION 2.8)


# Name of the project (will be the name of the plugin)
project(safex)

set (CMAKE_CXX_STANDARD 11)


file(GLOB BOOST_LIBS
    "deps/libboost*.a"
)

if(BUILD_WIN)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNODE_HOST_BINARY=node.exe")
    set(WIN_DELAY_LOAD_HOOK ${CMAKE_CURRENT_SOURCE_DIR}/win_delay_load_hook.cc)
    set(LIBNODE ${CMAKE_CURRENT_SOURCE_DIR}/deps/libnode.a)
endif()

file(GLOB SOURCE_FILES "src/*.cc" "src/*.h")
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES} ${WIN_DELAY_LOAD_HOOK})

# Gives our library file a .node extension without any "lib" prefix
set_target_properties(${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".node")

# Essential include files to build a node addon,
# You should add this line in every CMake.js based project
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_JS_INC} include/)

# INCLUDE N-API WRAPPERS
execute_process(COMMAND node -p "require('node-addon-api').include"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE NODE_ADDON_API_DIR
        )
string(REPLACE "\n" "" NODE_ADDON_API_DIR ${NODE_ADDON_API_DIR})
string(REPLACE "\"" "" NODE_ADDON_API_DIR ${NODE_ADDON_API_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${NODE_ADDON_API_DIR})

set(CMAKE_CXX_STANDARD_LIBRARIES " -std=c++11 ${CMAKE_CXX_STANDARD_LIBRARIES}")

# Essential library files to link to a node addon
# You should add this line in every CMake.js based project
target_link_libraries(${PROJECT_NAME} ${CMAKE_JS_LIB} ${CMAKE_THREAD_LIBS_INIT})


if(BUILD_WIN)
    target_link_libraries(${PROJECT_NAME}
        ${CMAKE_SOURCE_DIR}/deps/libwallet_api.dll.a
        ${LIBNODE}
        )
elseif(APPLE)
    target_link_libraries(${PROJECT_NAME}
        ${CMAKE_SOURCE_DIR}/deps/libwallet_merged.a
        ${CMAKE_SOURCE_DIR}/deps/libunbound.a
        ${CMAKE_SOURCE_DIR}/deps/libepee.a
        ${CMAKE_SOURCE_DIR}/deps/libeasylogging.a
        ${CMAKE_SOURCE_DIR}/deps/liblmdb.a
        ssl
        z
        crypto
        "-framework PCSC"
        ${BOOST_LIBS}
        )
else()
    target_link_libraries(${PROJECT_NAME}
        ${CMAKE_SOURCE_DIR}/deps/libwallet_merged.a
        ${CMAKE_SOURCE_DIR}/deps/libunbound.a
        ${CMAKE_SOURCE_DIR}/deps/libepee.a
        ${CMAKE_SOURCE_DIR}/deps/libeasylogging.a
        ${CMAKE_SOURCE_DIR}/deps/liblmdb.a
        ssl
        z
        crypto
        pcsclite
        ${BOOST_LIBS}
        )
endif()

