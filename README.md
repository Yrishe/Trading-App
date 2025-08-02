# Trading Desktop Application

A comprehensive Qt-based desktop trading platform with real-time market data, order management, and portfolio tracking capabilities.

## Features

### 🔐 Authentication System
- Secure user login with encrypted password storage
- SQLite database for user management
- Session management and user preferences

### 📊 Trading Interface
- Real-time market data display
- Order book visualization
- Buy/sell order placement (Market & Limit orders)
- Price change indicators with color coding
- Trading volume and bid/ask spread information

### 💰 Wallet Management
- Multi-currency portfolio tracking
- Real-time balance updates
- Transaction history
- Asset allocation visualization

### 📈 Market Analysis
- Candlestick chart visualization
- Historical price data
- Technical indicators
- Market trend analysis

### 📋 Order Management
- Order history tracking
- Order status monitoring
- Order modification and cancellation
- Trade execution logs

## Prerequisites

### System Requirements
- **Operating System**: macOS 10.15+ / Windows 10+ / Linux Ubuntu 18.04+
- **RAM**: Minimum 4GB, Recommended 8GB+
- **Storage**: 500MB free space

### Development Dependencies
- **Qt Framework**: 6.0 or higher
- **CMake**: 3.16 or higher
- **C++ Compiler**: GCC 9+ / Clang 10+ / MSVC 2019+
- **Git**: For version control

## Installation

### 1. Clone the Repository
```bash
git clone https://github.com/Yrishe/Trading-App.git
cd Trading-App
```

### 2. Install Qt Dependencies

#### macOS (using Homebrew)
```bash
brew install qt cmake
```

#### Ubuntu/Debian
```bash
sudo apt update
sudo apt install qt6-base-dev qt6-tools-dev cmake build-essential
```

#### Windows
- Download and install Qt from [qt.io](https://www.qt.io/download)
- Install CMake from [cmake.org](https://cmake.org/download/)
- Install Visual Studio with C++ support

### 3. Build the Application
```bash
mkdir build
cd build
cmake ..
make
```

### 4. Run the Application
```bash
./TradingDesktopApp
```

## Usage

### First Time Setup
1. **Launch the Application**: Run the executable from the build directory
2. **Create Account**: Click "Register" on the login dialog
3. **Login**: Enter your credentials to access the main interface

### Trading Operations
1. **View Market Data**: Real-time prices are displayed in the trading widget
2. **Place Orders**: 
   - Select order type (Market/Limit)
   - Enter quantity and price (for limit orders)
   - Click Buy/Sell to execute
3. **Monitor Portfolio**: Check your balances in the wallet widget
4. **Analyze Charts**: Use the candlestick chart for technical analysis

### Navigation
- **Trading Tab**: Main trading interface with order placement
- **Wallet Tab**: Portfolio overview and transaction history
- **Charts Tab**: Market analysis and price visualization
- **Orders Tab**: Order management and trade history

## Architecture

### Core Components

#### Authentication Layer
- `UserManager`: Handles user authentication and session management
- `User`: User data model with encrypted storage
- `Encryption`: Cryptographic utilities for secure data handling

#### Trading Engine
- `MerkelMain`: Core trading logic and market data processing
- `OrderBook`: Order matching and execution engine
- `OrderBookEntry`: Individual order representation
- `CSVReader`: Market data ingestion from CSV files

#### GUI Framework
- `MainWindow`: Primary application window and navigation
- `LoginDialog`: User authentication interface
- `TradingWidget`: Order placement and market data display
- `WalletWidget`: Portfolio management interface
- `CandlestickChart`: Price chart visualization
- `OrderWidget`: Order history and management

#### Data Models
- `CandleStick`: OHLCV price data structure
- `Wallet`: Portfolio and balance management
- SQLite database for persistent storage

### Design Patterns
- **MVC Architecture**: Separation of data, view, and control logic
- **Observer Pattern**: Real-time data updates across components
- **Factory Pattern**: Widget creation and initialization
- **Singleton Pattern**: Database connection management

## Configuration

### Market Data
- Historical data files are located in the `data/` directory
- Supported formats: CSV with OHLCV structure
- Real-time data can be configured via API endpoints

### Database
- SQLite database is automatically created on first run
- Location: `trading_app.db` in the application directory
- Schema includes users, orders, and transaction tables

## Development

### Project Structure
```
Trading_App_Console/
├── Include/           # Header files
│   ├── Auth/         # Authentication components
│   ├── GUI/          # User interface headers
│   └── Crypto/       # Encryption utilities
├── src/              # Source code implementation
│   ├── Auth/         # Authentication logic
│   ├── GUI/          # UI component implementations
│   └── Crypto/       # Cryptographic functions
├── data/             # Market data files
├── build/            # Build artifacts (generated)
└── CMakeLists.txt    # Build configuration
```

### Building for Development
```bash
# Debug build
cmake -DCMAKE_BUILD_TYPE=Debug ..
make

# Release build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

### Running Tests
```bash
# Unit tests (if implemented)
make test
```

## Troubleshooting

### Common Issues

#### Application Crashes on Startup
- **Solution**: Ensure Qt libraries are properly installed and accessible
- **Check**: Run `ldd TradingDesktopApp` (Linux) or similar to verify dependencies

#### Database Connection Errors
- **Solution**: Verify write permissions in the application directory
- **Check**: Ensure SQLite is properly installed

#### Market Data Not Loading
- **Solution**: Verify CSV files are present in the `data/` directory
- **Check**: Ensure proper file permissions and format

#### Login Issues
- **Solution**: Delete `trading_app.db` to reset the database
- **Note**: This will remove all user accounts and data

### Debug Mode
To enable debug output:
```bash
export QT_LOGGING_RULES="*.debug=true"
./TradingDesktopApp
```

## Contributing

### Development Guidelines
1. **Code Style**: Follow Qt coding conventions
2. **Documentation**: Comment complex algorithms and business logic
3. **Testing**: Add unit tests for new features
4. **Security**: Never commit sensitive data or credentials

### Submitting Changes
1. Fork the repository
2. Create a feature branch: `git checkout -b feature-name`
3. Commit changes: `git commit -m "Description"`
4. Push to branch: `git push origin feature-name`
5. Submit a pull request

## Security Considerations

- **Password Storage**: Uses bcrypt hashing with salt
- **Database**: SQLite with prepared statements to prevent injection
- **Memory**: Sensitive data is cleared after use
- **Network**: All API communications should use HTTPS

## Support

For support and questions:
- **Issues**: [GitHub Issues](https://github.com/Yrishe/Trading_App_Console/issues)
- **Documentation**: This README and inline code comments
- **Community**: Discussions tab on GitHub repository

---

**Note**: This application is for educational and demonstration purposes. Do not use with real trading accounts or live market data without proper testing and risk management.
