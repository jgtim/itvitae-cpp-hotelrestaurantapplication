cmake_minimum_required(VERSION 3.16)

project(MolvenoLakeHotelResort VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOMOC_PATH_PREFIX ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Sql)

add_subdirectory(QXlsx)

# Automatically pick up every .cpp/.h/.ui file under the project root.
# CONFIGURE_DEPENDS tells CMake to re-check the glob on every build
file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.ui"
)

# Exclude things that aren't part of this target's own sources:
# - the QXlsx subdirectory (it has its own CMakeLists.txt / target)
# - the *current* build directory, in case of in-source or nested builds
# - ANY other build directory that happens to be sitting in the tree
list(FILTER PROJECT_SOURCES EXCLUDE REGEX "${CMAKE_CURRENT_SOURCE_DIR}/QXlsx/.*")
list(FILTER PROJECT_SOURCES EXCLUDE REGEX "${CMAKE_CURRENT_BINARY_DIR}/.*")
if(DEFINED CMAKE_BINARY_DIR)
    list(FILTER PROJECT_SOURCES EXCLUDE REGEX "${CMAKE_BINARY_DIR}/.*")
endif()
list(FILTER PROJECT_SOURCES EXCLUDE REGEX "/tests/.*")
list(FILTER PROJECT_SOURCES EXCLUDE REGEX "/temp-example-proj-dir/.*")
list(FILTER PROJECT_SOURCES EXCLUDE REGEX "/(build|cmake-build-[^/]*)/")
list(FILTER PROJECT_SOURCES EXCLUDE REGEX "/(CMakeFiles|.*_autogen)/")

set(APP_SOURCES ${PROJECT_SOURCES})
list(REMOVE_ITEM APP_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/main/main.cpp")

add_library(MolvenoLakeHotelResortCore STATIC ${APP_SOURCES})
target_include_directories(MolvenoLakeHotelResortCore PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(MolvenoLakeHotelResortCore PUBLIC
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Sql
    QXlsx::QXlsx
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(MolvenoLakeHotelResort
        MANUAL_FINALIZATION
        main/main.cpp
    )

# Define target properties for Android with Qt 6 as:
#    set_property(TARGET MolvenoLakeHotelResort APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(MolvenoLakeHotelResort SHARED
            main/main.cpp
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(MolvenoLakeHotelResort
            main/main.cpp
        )
    endif()
endif()

target_include_directories(MolvenoLakeHotelResort PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(MolvenoLakeHotelResort PRIVATE
    MolvenoLakeHotelResortCore
)

add_custom_command(TARGET MolvenoLakeHotelResort POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/resources"
    "$<TARGET_FILE_DIR:MolvenoLakeHotelResort>/resources"
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.MolvenoLakeHotelResort)
endif()
set_target_properties(MolvenoLakeHotelResort PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS MolvenoLakeHotelResort
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(MolvenoLakeHotelResort)
endif()

include(CTest)
if(BUILD_TESTING)
    include(FetchContent)

    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.18.0
    )
    FetchContent_MakeAvailable(googletest)

    file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp"
    )

    add_executable(MolvenoLakeHotelResortTests ${TEST_SOURCES})
    target_link_libraries(MolvenoLakeHotelResortTests PRIVATE
        MolvenoLakeHotelResortCore
        GTest::gtest_main
    )

    include(GoogleTest)
    gtest_discover_tests(MolvenoLakeHotelResortTests)
endif()