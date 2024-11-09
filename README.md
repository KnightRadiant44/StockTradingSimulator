# Stock Trading Simulator

## Description

A Qt-based trading simulator that allows users to test different trading strategies in a simulated market environment. The simulator features automated trading algorithms, interactive price charts, and real-time performance tracking.

## Features

- **Multiple Trading Strategies:**
  - Buy and Hold
  - Mean Reversion
  - Trend Following
  - Moving Average
  - Random Strategy

- **Risk Management:**
  - Dynamic position sizing
  - Volatility adjustments
  - Maximum loss limits
  - Market event adaptation

- **Real-time Visualization:**
  - Stock price history charts
  - Balance history tracking
  - Performance metrics display
  - Trading activity logs

- **Market Simulation:**
  - Dynamic price generation
  - Market event simulation
  - Transaction fees and slippage
  - Volatility adjustments

## Performance Metrics
- Total return calculation
- Maximum drawdown tracking
- Volatility measurement
- Sharpe ratio analysis
- Trade history logging

## Project Structure

- `TradingBot`: Main simulation controller
- `TradingStrategy`: Base class for trading strategies
- `Market`: Market simulation and price generation
- `RiskManagement`: Position sizing and risk control
- `DirectionalStrategy`: Base class for trend-based strategies
- `MarketNeutralStrategy`: Base class for market-neutral strategies

## Data Storage

The application stores simulation data in the Documents folder under "TradingSimulation":
- Trade history logs
- Performance charts
- Session state data
- Balance history

## Requirements

### Prerequisites
- MacOS operating system
- Qt Creator 6.7.3 or higher
- Qt Charts module

### Dependencies
- QtCharts
- Qt Core libraries
- C++11 or higher

## Installation

### Installing Qt Creator

1. **Download Qt Creator:**
   - Visit the Qt Downloads page and download the installer for your operating system.

2. **Install Qt Creator:**
   - Run the installer and follow the on-screen instructions.
   - During installation, ensure you select the components you need, including the Qt Charts module.

### Installing Qt Charts

Qt Charts is typically included with the Qt installation. Ensure you select it during the Qt Creator installation process. If you need to install it separately:

1. **Open Qt Maintenance Tool:**
   - Navigate to the directory where Qt is installed and run the `MaintenanceTool`.

2. **Add or Update Components:**
   - Select "Add or remove components."
   - Under the "Qt" section, ensure "Qt Charts" is checked.
   - Click "Next" and follow the instructions to complete the installation.

## Running the Project

1. **Open the Project:**
   - Launch Qt Creator.
   - Open the project by selecting `File > Open File or Project` and navigate to your project's `CMakeLists.txt` file.

2. **Build the Project:**
   - Click on the "Build" button or press `Cmd + B` to compile the project.

3. **Run the Project:**
   - Click on the "Run" button or press `Cmd + R` to execute the project.