cmake_minimum_required(VERSION 3.16)
project(Trading VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Charts)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Charts)

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    logindialog.cpp
    logindialog.h
    Market.h
    TradingBot.h
    TradingBot.cpp
    MarketNeutralStrategy.h
    BuyAndHold.h
    DirectionalStrategy.h
    helpwindow.h
    MeanReversion.h
    MovingAvg.h
    RandomStrategy.h
    RiskManagement.h
    TradingStrategy.h
    TrendFollowing.h
)

# Main Trading executable
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Trading
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    add_executable(Trading
        ${PROJECT_SOURCES}
    )
endif()

target_link_libraries(Trading PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Charts
)

set_target_properties(Trading PROPERTIES
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# Test executable
add_executable(trading_bot_test
    trading_bot_test_.cpp
    TradingBot.cpp
    # Add other necessary source files for the test here
)

target_link_libraries(trading_bot_test PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Charts
)

# Include directories for both executables
target_include_directories(Trading PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(trading_bot_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

include(GNUInstallDirs)
install(TARGETS Trading
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Trading)
endif()
