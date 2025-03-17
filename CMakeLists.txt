cmake_minimum_required(VERSION 3.20)

project(MixUtilities VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(log4cxx)
find_package(fmt)

# https://github.com/lballabio/QuantLib/issues/2088#issuecomment-2405001198
if (POLICY CMP0167)
    # The FindBoost module is removed in cmake 3.30, finding the upstream BoostConfig.cmake
    find_package(Boost CONFIG REQUIRED COMPONENTS iostreams)
else()
    find_package(Boost REQUIRED COMPONENTS iostreams)
endif()

set(PROJECT_SOURCES
        main.cpp
        rmmixviewer.cpp
        rmmixviewer.h
        rmmixviewer.ui
        mixfile.h mixfile.cpp
        mixfilemodel.h mixfilemodel.cpp
        mixfiledisplay.h mixfiledisplay.cpp mixfiledisplay.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(MixUtilities
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET MixUtilities APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(MixUtilities SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(MixUtilities
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(MixUtilities PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Boost::iostreams
    log4cxx
    fmt::fmt
)
target_include_directories(MixUtilities PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    dds_image/include
)
target_compile_options(MixUtilities PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>: /Zc:preprocessor>
)

set_target_properties(MixUtilities PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS MixUtilities
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(MixUtilities)
endif()

if(WIN32)
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
            find_program(TOOL_WINDEPLOYQT NAMES windeployqt.debug.bat)
    else()
            find_program(TOOL_WINDEPLOYQT NAMES windeployqt)
    endif()

    add_custom_command(TARGET MixUtilities POST_BUILD
        COMMAND ${TOOL_WINDEPLOYQT}
                $<TARGET_FILE:MixUtilities>
        COMMENT "Running windeployqt..."
    )
endif()
