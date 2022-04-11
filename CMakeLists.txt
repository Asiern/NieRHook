cmake_minimum_required(VERSION 3.7)
project(NieRHook VERSION 2.0 DESCRIPTION "NieR:Automata Memory Hook")

set(BUILD_TYPE "Release")
option(NIERHOOK_STATIC_LIB "Build static library" ON)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

# Configure library
set(
    SOURCES 
    src/NieRHook.cpp
    include/NieRHook.hpp
    include/Offsets.hpp
    )
if(NIERHOOK_STATIC_LIB)
    add_library(${PROJECT_NAME} STATIC ${SOURCES})
else()
    add_library(${PROJECT_NAME} SHARED ${SOURCES})
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER include/NieRHook.h)
target_include_directories(${PROJECT_NAME} PRIVATE include)
target_include_directories(${PROJECT_NAME} PRIVATE src)

# Configure Demo
if(BUILD_TYPE STREQUAL "Debug")
    set(DEMO_SOURCES src/Main.cpp)
    add_executable(${PROJECT_NAME}_DEMO ${DEMO_SOURCES})
    target_include_directories(${PROJECT_NAME}_DEMO PRIVATE include)
    target_link_libraries(${PROJECT_NAME}_DEMO ${PROJECT_NAME})
endif()
