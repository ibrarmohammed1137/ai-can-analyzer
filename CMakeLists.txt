cmake_minimum_required(VERSION 3.20)

project(ai-can-analyzer
    VERSION 1.0.0
    DESCRIPTION "Interpret CAN logs and detect unusual patterns"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)
endif()

option(AICAN_BUILD_TESTS     "Build unit tests"         ON)
option(AICAN_BUILD_EXAMPLES  "Build examples"           ON)
option(AICAN_ENABLE_ASAN     "Enable sanitizers"        OFF)
option(AICAN_ENABLE_WERROR   "Treat warnings as errors" OFF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(CompilerWarnings)
include(Sanitizers)

add_library(aican
    src/frame.cpp
    src/anomaly.cpp
    src/analyzer.cpp
    src/parser.cpp
    src/report.cpp
    src/detectors/new_id.cpp
    src/detectors/frequency.cpp
    src/detectors/payload_range.cpp
    src/detectors/sequence.cpp
    src/detectors/bus_load.cpp
)

target_include_directories(aican
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)

target_compile_features(aican PUBLIC cxx_std_17)
aican_set_warnings(aican)
aican_enable_sanitizers(aican)

find_package(Threads REQUIRED)
target_link_libraries(aican PUBLIC Threads::Threads)

add_executable(aican_cli src/main.cpp)
target_link_libraries(aican_cli PRIVATE aican)
set_target_properties(aican_cli PROPERTIES OUTPUT_NAME aican)

if(AICAN_BUILD_EXAMPLES)
    add_executable(synth_log examples/synth_log.cpp)
    target_link_libraries(synth_log PRIVATE aican)
endif()

if(AICAN_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

include(GNUInstallDirs)
install(TARGETS aican aican_cli
    EXPORT aicanTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

include(cmake/PackageProject.cmake)
