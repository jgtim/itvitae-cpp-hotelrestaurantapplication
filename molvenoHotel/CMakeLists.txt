cmake_minimum_required(VERSION 3.19)
project(molvenoHotel LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

qt_add_executable(molvenoHotel
    WIN32
    MACOSX_BUNDLE

    main/main.cpp

    launcher/launcher.cpp
    launcher/launcher.h
    launcher/launcher.ui

    # Hotel module
    # hotel/hotel.cpp
    # hotel/hotel.h
    # hotel/hotel.ui

    # Restaurant module
    # restaurant/restaurant.cpp
    # restaurant/restaurant.h
    # restaurant/restaurant.ui
)

target_link_libraries(molvenoHotel
    PRIVATE
        Qt::Core
        Qt::Widgets
)

target_include_directories(molvenoHotel PRIVATE
    launcher
    hotel
    restaurant
)

install(TARGETS molvenoHotel
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET molvenoHotel
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
