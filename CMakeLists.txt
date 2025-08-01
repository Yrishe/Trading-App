cmake_minimum_required(VERSION 3.16)
project(TradingDesktopApp VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required Qt6 components
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Charts Sql)

# Find OpenSSL for encryption
find_package(OpenSSL REQUIRED)

# Include directories
include_directories(Include)
include_directories(Include/GUI)
include_directories(Include/Auth)
include_directories(Include/Crypto)

# Source files
set(SOURCES
    src/main.cpp
    src/MerkelMain.cpp
    src/OrderBook.cpp
    src/OrderBookEntry.cpp
    src/CSVReader.cpp
    src/Wallet.cpp
    src/CandleStick.cpp
    
    # GUI sources
    src/GUI/MainWindow.cpp
    src/GUI/LoginDialog.cpp
    src/CandlestickChart.cpp
    src/TradingWidget.cpp
    src/WalletWidget.cpp
    src/OrderWidget.cpp
    
    # Authentication sources
    src/Auth/UserManager.cpp
    src/Auth/User.cpp
    
    # Crypto sources
    src/Crypto/Encryption.cpp
)

# Header files
set(HEADERS
    Include/MerkelMain.h
    Include/OrderBook.h
    Include/OrderBookEntry.h
    Include/CSVReader.h
    Include/Wallet.h
    Include/CandleStick.h
    
    # GUI headers
    Include/GUI/MainWindow.h
    Include/GUI/LoginDialog.h
    Include/GUI/TradingWidget.h
    Include/GUI/CandlestickChart.h
    Include/GUI/WalletWidget.h
    Include/GUI/OrderWidget.h
    
    # Authentication headers
    Include/Auth/UserManager.h
    Include/Auth/User.h
    
    # Crypto headers
    Include/Crypto/Encryption.h
)

# Create executable
add_executable(TradingDesktopApp ${SOURCES} ${HEADERS})

# Link Qt6 libraries
target_link_libraries(TradingDesktopApp
    Qt6::Core
    Qt6::Widgets
    Qt6::Charts
    Qt6::Sql
    OpenSSL::SSL
    OpenSSL::Crypto
)

# Set Qt6 properties
set_target_properties(TradingDesktopApp PROPERTIES
    AUTOMOC ON
    AUTORCC ON
    AUTOUIC ON
)

# Copy data files to build directory
file(COPY ${CMAKE_SOURCE_DIR}/data DESTINATION ${CMAKE_BINARY_DIR})