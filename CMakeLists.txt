cmake_minimum_required(VERSION 3.16)

project(MolvenoLakeHotelResort VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Sql)

add_subdirectory(QXlsx)

set(PROJECT_SOURCES
        main/main.cpp

        hotel/mainwindow.cpp
        hotel/mainwindow.h
        hotel/mainwindow.ui

        hotel/reservationstab/reservationstab.h
        hotel/reservationstab/reservationstab.cpp
        hotel/reservationstab/reservationstab.ui

        hotel/reservationstab/editreservationdialog.h
        hotel/reservationstab/editreservationdialog.cpp
        hotel/reservationstab/editreservationdialog.ui

        hotel/roomswindow/roomswindow.h
        hotel/roomswindow/roomswindow.cpp
        hotel/roomswindow/roomswindow.ui

        hotel/restaurantwindow/restaurantwindow.h
        hotel/restaurantwindow/restaurantwindow.cpp
        hotel/restaurantwindow/restaurantwindow.ui

        readers/hotelkamersreader.h
        readers/hotelkamersreader.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(MolvenoLakeHotelResort
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        hotel/restaurantwindow/Restaurant/table.h
        hotel/roomswindow/roomdetailsdialog.h hotel/roomswindow/roomdetailsdialog.cpp hotel/roomswindow/roomdetailsdialog.ui

        hotel/reservationstab/Reservation/reservation.h
        hotel/reservationstab/Reservation/address.h
        hotel/reservationstab/Reservation/gender.h
        hotel/reservationstab/Reservation/address.cpp
        hotel/roomswindow/Room/room.h
        hotel/roomswindow/Room/room.cpp
        hotel/database/databaseutils.h
        hotel/database/databaseutils.cpp
        hotel/roomswindow/confirmdialog.h hotel/roomswindow/confirmdialog.cpp hotel/roomswindow/confirmdialog.ui
        readers/reservationserializer.h
        readers/reservationserializer.cpp
        hotel/reservationstab/Guest/guest.h
        hotel/reservationstab/Guest/IDType.h
        hotel/reservationstab/Guest/PaymentMethod.h
        hotel/reservationstab/Reservation/PaymentStatus.h
        hotel/restaurantwindow/restaurant_mockdata.h
        hotel/restaurantwindow/Restaurant/table.h
        filter/multicolumnfilterproxymodel.h filter/multicolumnfilterproxymodel.cpp
    )

# Define target properties for Android with Qt 6 as:
#    set_property(TARGET MolvenoLakeHotelResort APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(MolvenoLakeHotelResort SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(MolvenoLakeHotelResort
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(MolvenoLakeHotelResort PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    QXlsx::QXlsx
    Qt${QT_VERSION_MAJOR}::Sql
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
