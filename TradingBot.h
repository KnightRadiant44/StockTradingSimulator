#ifndef TRADINGBOT_H
#define TRADINGBOT_H

#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <algorithm>
#include <iomanip>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include <numeric>
#include <algorithm>
#include <QObject>
#include "TradingStrategy.h"
#include "RiskManagement.h"
#include "Market.h"
#include "BuyAndHold.h"
#include "Market.h"
#include "MeanReversion.h"
#include "MovingAvg.h"
#include "RandomStrategy.h"
#include "RiskManagement.h"
#include "TradingStrategy.h"
#include "TrendFollowing.h"
#include <QObject>
#include <QMutex>

class TradingBot : public QObject {
    Q_OBJECT

private:
    mutable QMutex mutex;
    QString tradesFilePath;
    std::string lastMarketEvent;
    std::vector<TradingStrategy*> strategies;
    TradingStrategy* currentStrategy;
    double balance;
    int ownedStocks;
    std::vector<double> balanceHistory;
    Market market;
    RiskManagement* riskManager;
    double lastUpdateValue;
    int lastUpdateDay;
    double initialTotalValue;
    int buysSinceLastUpdate, sellsSinceLastUpdate, holdsSinceLastUpdate;
    int totalBuys, totalSells, totalHolds;
    int currentDay;
    int totalDays;

    const double transactionFeePercentage = 0.005;
    const double slippageFactor = 0.001;

    double maxDrawdown;
    double totalReturn;
    double volatility;
    double sharpeRatio;

    std::string generateMarketEvent();


public:
    TradingBot(QObject *parent = nullptr);
    // getters for varibles
    double getCurrentPrice() const { return market.getPrice(); }
    double getTotalValue() const { return balance + ownedStocks * market.getPrice(); }
    const std::string& getLastMarketEvent() const { return lastMarketEvent; }
    double getBalance() const { return balance; }
    int getOwnedStocks() const { return ownedStocks; }
    double getTotalReturn() const { return totalReturn; }
    double getMaxDrawdown() const { return maxDrawdown; }
    double getVolatility() const { return volatility; }
    double getMarketVolatility() const { return market.getVolatility(); }
    double getSharpeRatio() const { return sharpeRatio; }
    std::string getCurrentStrategyName() const;
    double getProfitLossSinceStart() const { return getTotalValue() - initialTotalValue; }
    double getProfitLossSinceLastUpdate() const { return getTotalValue() - lastUpdateValue; }
    int getCurrentDay() const { return currentDay; }
    int getLastBuys() const { return buysSinceLastUpdate; }
    int getLastSells() const { return sellsSinceLastUpdate; }
    int getLastHolds() const { return holdsSinceLastUpdate; }
    int getTotalBuys() const { return totalBuys; }
    int getTotalSells() const { return totalSells; }
    int getTotalHolds() const { return totalHolds; }

    void initializeSimulation(int days);
    void executeNextDay();
    bool isSimulationComplete() const { return currentDay > totalDays; }
    void resetActionCounts();

signals:
    void updateUI();
    void simulationComplete();


                                                                                    // Displays the update information for each day in the trading simulation
    void displayUpdate(int day, double currentPrice, double totalValue, const std::string& marketEvent) {
        std::cout << "\n--- Update for Day " << day << " ---" << std::endl;         // Day header
        std::cout << "Current price: $" << std::fixed << std::setprecision(2) << currentPrice << std::endl; // Current market price
        std::cout << "Current balance: $" << balance << std::endl;                  // Current monetary balance
        std::cout << "Owned stocks: " << ownedStocks << " (valued at $" << ownedStocks * currentPrice << ")" << std::endl; // Owned stocks and their value
        std::cout << "Total current value: $" << totalValue << std::endl;           // Total current value

        std::cout << "Actions since last update: " << buysSinceLastUpdate << " buys, "
                  << sellsSinceLastUpdate << " sells, " << holdsSinceLastUpdate << " holds" << std::endl; // Recent actions
        std::cout << "Total actions: " << totalBuys << " buys, "
                  << totalSells << " sells, " << totalHolds << " holds" << std::endl; // Total actions

                                                                                    // Calculate and display profit/loss since last update, if applicable
        if (lastUpdateDay > 0) {
            double profitSinceLastUpdate = totalValue - lastUpdateValue;            // Profit/loss since last update
            std::cout << "Profit/Loss since last update: $" << profitSinceLastUpdate
                      << " (" << (profitSinceLastUpdate / lastUpdateValue * 100) << "%)" << std::endl;
        }

        double profitSinceStart = totalValue - initialTotalValue;                   // Profit/loss since start
        std::cout << "Profit/Loss since start: $" << profitSinceStart
                  << " (" << (profitSinceStart / initialTotalValue * 100) << "%)" << std::endl;

        std::cout << "Current strategy: " << currentStrategy->getName() << std::endl; // Current strategy name
        std::cout << "Market volatility: " << market.getVolatility() * 100 << "%" << std::endl; // Market volatility percentage
        std::cout << "Market event: " << marketEvent << std::endl;                  // Last market event

        lastUpdateValue = totalValue;                                               // Update last recorded value for future calculations
        lastUpdateDay = day;                                                        // Update the day for tracking
    }

                                                                                    // Apply transaction fee to a given amount and return the net amount after fee deduction
    double applyTransactionFee(double amount) {
        return amount * (1.0 - transactionFeePercentage);                           // Deduct the transaction fee
    }

                                                                                    // Apply slippage to the current price and return the adjusted price
    double applySlippage(double currentPrice) {
        double slippage = currentPrice * slippageFactor * ((std::rand() % 2 == 0) ? 1 : -1); // Randomly add/subtract slippage
        return currentPrice + slippage;                                             // Return adjusted price
    }


                                                                                    // Calculate the maximum drawdown over the balance history
    double calculateMaxDrawdown() {
        double peak = balanceHistory[0];                                            // Start with the initial balance as the peak
        double maxDrawdown = 0;                                                     // Initialize max drawdown
        for (double value : balanceHistory) {
            peak = std::max(peak, value);                                           // Update peak if current value is higher
            double drawdown = (peak - value) / peak;                                // Calculate drawdown as a fraction
            maxDrawdown = std::max(maxDrawdown, drawdown);                          // Update max drawdown if needed
        }
        return maxDrawdown;                                                         // Return the maximum drawdown found
    }

                                                                                    // Calculate the cumulative return based on balance history
    double calculateCumulativeReturn() {
        double finalValue = balanceHistory.back();                                  // Get the last recorded balance
        return (finalValue - initialTotalValue) / initialTotalValue;                // Calculate cumulative return
    }

                                                                                    // Calculate volatility (standard deviation of returns)
    double calculateVolatility() {
        std::vector<double> returns;                                                // Vector to store daily returns
        for (size_t i = 1; i < balanceHistory.size(); ++i) {
                                                                                    // Calculate daily return as the percentage change from the previous day's balance
            double dailyReturn = (balanceHistory[i] - balanceHistory[i - 1]) / balanceHistory[i - 1];
            returns.push_back(dailyReturn);                                         // Store daily return in the vector
        }

                                                                                    // Calculate the mean return over the recorded returns
        double meanReturn = std::accumulate(returns.begin(), returns.end(), 0.0) / returns.size(); // Mean return calculation
        double variance = 0;                                                        // Initialize variance to zero

                                                                                    // Calculate the variance of the returns
        for (double r : returns) {
            variance += std::pow(r - meanReturn, 2);                                // Accumulate squared differences from the mean
        }
        variance /= returns.size();                                                 // Calculate the average variance

        return std::sqrt(variance);                                                 // Return the standard deviation (volatility)
    }

                                                                                    // Calculate Sharpe Ratio
    double calculateSharpeRatio(double riskFreeRate = 0.0) {
                                                                                    // Calculate mean return based on cumulative return and the number of recorded balances
        double meanReturn = calculateCumulativeReturn() / balanceHistory.size();    // Mean return
                                                                                    // Return the Sharpe Ratio calculated as (Mean Return - Risk-Free Rate) / Volatility
        return (meanReturn - riskFreeRate) / volatility;
    }

public:
    // Constructor for TradingBot
    TradingBot() : currentStrategy(nullptr), balance(10000), ownedStocks(0),
        riskManager(new RiskManagement(0.02, 0.1)),                                 // Initialize risk management with parameters
        lastUpdateValue(10000), lastUpdateDay(0), initialTotalValue(10000),
        buysSinceLastUpdate(0), sellsSinceLastUpdate(0), holdsSinceLastUpdate(0),
        totalBuys(0), totalSells(0), totalHolds(0),
        maxDrawdown(0), totalReturn(0), volatility(0), sharpeRatio(0) {
    // Add various trading strategies to the strategies vector
        strategies.push_back(new BuyAndHoldStrategy());
        strategies.push_back(new MeanReversionStrategy());
        strategies.push_back(new TrendFollowingStrategy());
        strategies.push_back(new RandomStrategy());
        strategies.push_back(new MovingAverageStrategy());
    }

    // Destructor for TradingBot
    ~TradingBot() {
        for (auto strategy : strategies) {
            delete strategy;                                                        // Clean up memory for each strategy
        }
        delete riskManager;                                                         // Clean up memory for the risk manager
    }

// Set the current trading strategy by index
    void setStrategy(int index) {
        if (index >= 0 && index < strategies.size()) {                              // Ensure index is valid
            currentStrategy = strategies[index];                                    // Set current strategy
        }
    }

    // Execute the selected trading strategy over a specified number of days
    void executeStrategy(int days) {
        if (!currentStrategy) {                                                     // Check if a strategy has been set
            std::cout << "Please select a strategy first." << std::endl;            // Prompt for strategy selection
            return;
        }

        initialTotalValue = balance + ownedStocks * market.getPrice();              // Calculate total value at the start
        std::cout << "Starting balance: $" << balance << std::endl;                 // Display starting balance
        std::cout << "Starting owned stocks: " << ownedStocks << " (valued at $" << ownedStocks * market.getPrice() << ")" << std::endl; // Display owned stocks
        std::cout << "Total starting value: $" << initialTotalValue << std::endl;   // Display total starting value
        std::cout << "Using strategy: " << currentStrategy->getName() << std::endl; // Display current strategy name

        // Ask user for update frequency during the simulation
        std::cout << "\nHow often do you want to see updates? (Enter number of days): ";
        int updateFrequency;                                                        // Variable for user-defined update frequency
        std::cin >> updateFrequency;                                                // Input update frequency
        std::cin.ignore();                                                          // Clear input buffer

        balanceHistory.clear();                                                     // Clear any existing balance history
        balanceHistory.push_back(initialTotalValue);                                // Record initial total value in history
        std::ofstream dataFile("balance_history.txt");                              // File to record balance history
        std::ofstream tradesFile("trades_taken.txt");                               // File to log trades taken

        buysSinceLastUpdate = sellsSinceLastUpdate = holdsSinceLastUpdate = 0;      // Reset action counters
        totalBuys = totalSells = totalHolds = 0;                                    // Reset total action counters

        for (int day = 1; day <= days; ++day) {                                     // Loop over each day in the simulation
            double currentPrice = market.getPrice();                                // Get current market price
            std::string marketEvent = generateMarketEvent();                        // Generate a market event for the day

            market.adjustVolatility(marketEvent);                                   // Adjust market volatility based on the event
            market.adjustOverallTrend(marketEvent);                                 // Adjust overall market trend based on the event
            riskManager->adjustRiskTolerance(marketEvent);                          // Adjust risk tolerance based on the market event

            double action = currentStrategy->execute(currentPrice, marketEvent);    // Execute strategy to get action
            double adjustedAction = riskManager->adjustPosition(action, currentPrice, balance); // Adjust action based on risk management

            currentPrice = applySlippage(currentPrice);                             // Apply slippage to current price

            std::string tradeAction;                                                // Variable to store the action taken
            double actionTaken = 0;                                                 // Variable to track the amount of action taken

            if (adjustedAction > 0) {                                               // If action indicates buying
                int stocksToBuy = static_cast<int>(adjustedAction * balance / currentPrice); // Calculate number of stocks to buy
                double cost = applyTransactionFee(stocksToBuy * currentPrice);      // Calculate total cost including transaction fee
                if (balance >= cost) {                                              // Check if sufficient funds are available
                    ownedStocks += stocksToBuy;                                     // Increase owned stocks
                    balance -= cost;                                                // Deduct cost from balance
                    tradeAction = "Buy";                                            // Record action taken
                    actionTaken = stocksToBuy;                                      // Store amount of stocks bought
                    buysSinceLastUpdate++;                                          // Increment buys counter
                    totalBuys++;                                                    // Increment total buys counter
                } else {                                                            // Insufficient funds to buy
                    tradeAction = "Hold (Insufficient funds)";                      // Record action taken
                    holdsSinceLastUpdate++;                                         // Increment holds counter
                    totalHolds++;                                                   // Increment total holds counter
                }
            } else if (adjustedAction < 0 && ownedStocks > 0) {                     // If action indicates selling
                int stocksToSell = std::min(static_cast<int>(-adjustedAction * ownedStocks), ownedStocks); // Calculate number of stocks to sell
                double revenue = applyTransactionFee(stocksToSell * currentPrice);  // Calculate revenue from selling stocks
                ownedStocks -= stocksToSell;                                        // Decrease owned stocks
                balance += revenue;                                                 // Add revenue to balance
                tradeAction = "Sell";                                               // Record action taken
                actionTaken = stocksToSell;                                         // Store amount of stocks sold
                sellsSinceLastUpdate++;                                             // Increment sells counter
                totalSells++;                                                       // Increment total sells counter
            } else {                                                                // If no action is taken
                tradeAction = "Hold";                                               // Record action taken
                holdsSinceLastUpdate++;                                             // Increment holds counter
                totalHolds++;                                                       // Increment total holds counter
            }

            double totalValue = balance + ownedStocks * currentPrice;               // Calculate total value after action
            balanceHistory.push_back(totalValue);                                   // Add total value to history

            // Log trade details to file
            tradesFile << "Day " << day << ": Price = $" << currentPrice
                       << ", Action = " << tradeAction << " " << actionTaken
                       << " stocks, Balance = $" << balance
                       << ", Owned Stocks = " << ownedStocks << "\n";

            dataFile << day << " " << totalValue << std::endl;                      // Log daily total value to file

            // Check if it's time to display updates
            if (day % updateFrequency == 0 || day == days) {
                displayUpdate(day, currentPrice, totalValue, marketEvent);          // Display update to user

                if (day < days) {                                                   // If not the last day
                    std::cout << "\nDo you want to change the strategy? (y/n): ";   // Ask user if they want to change strategy
                    char choice;                                                    // Variable to store user choice
                    std::cin >> choice;                                             // Input user choice
                    std::cin.ignore();                                              // Clear input buffer

                    if (choice == 'y' || choice == 'Y') {                           // If user wants to change strategy
                        displayStrategies();                                        // Display available strategies
                        std::cout << "Select new strategy (1-5): ";                 // Prompt user to select a new strategy
                        int strategyChoice;                                         // Variable for strategy choice
                        std::cin >> strategyChoice;                                 // Input strategy choice
                        setStrategy(strategyChoice - 1);                            // Set the new strategy
                        std::cout << "Strategy changed to: " << currentStrategy->getName() << std::endl; // Confirm change
                    }

                    std::cout << "\nPress Enter to continue the simulation.";       // Prompt user to continue
                    std::cin.get();                                                 // Wait for user input
                }

                // Reset action counters for the next update
                buysSinceLastUpdate = sellsSinceLastUpdate = holdsSinceLastUpdate = 0;
            }
        }

        tradesFile.close();                                                         // Close trades log file
        dataFile.close();                                                           // Close balance history file

        // Final calculations and summary output
        double finalValue = balance + ownedStocks * market.getPrice();              // Calculate final total value
        double profit = finalValue - initialTotalValue;                             // Calculate total profit/loss
        // std::cout << "\nSimulation complete!" << std::endl;
        // std::cout << "Final balance: $" << balance << std::endl;                    // Display final balance
        // std::cout << "Owned stocks: " << ownedStocks << " (valued at $" << ownedStocks * market.getPrice() << ")" << std::endl; // Display owned stocks
        // std::cout << "Total final value: $" << finalValue << std::endl; // Display total final value
        // std::cout << "Total " << (profit >= 0 ? "profit" : "loss") << ": $" << std::abs(profit) << std::endl; // Display total profit/loss

        // Calculate and display performance metrics
        maxDrawdown = calculateMaxDrawdown();                                       // Calculate maximum drawdown
        totalReturn = calculateCumulativeReturn();                                  // Calculate total return
        volatility = calculateVolatility();                                         // Calculate volatility
        sharpeRatio = calculateSharpeRatio();                                       // Calculate Sharpe Ratio

        // std::cout << "\nPerformance Metrics:" << std::endl;                         // Display performance metrics
        // std::cout << "Cumulative Return: " << totalReturn * 100 << "%" << std::endl;
        // std::cout << "Maximum Drawdown: " << maxDrawdown * 100 << "%" << std::endl;
        // std::cout << "Volatility: " << volatility * 100 << "%" << std::endl;
        // std::cout << "Sharpe Ratio: " << sharpeRatio << std::endl;

        // Generate balance history chart using gnuplot
        system("gnuplot -p -e \"plot 'balance_history.txt' using 1:2 with lines title 'Total Value Over Days';\"");
        std::cout << "Balance chart displayed in window." << std::endl;
        std::cout << "Detailed trades saved in 'trades_taken.txt'." << std::endl;
    }

    // Display available trading strategies
    // void displayStrategies() {
    //     std::cout << "Available strategies:" << std::endl;                          // Inform user of available strategies
    //     for (int i = 0; i < strategies.size(); ++i) {                               // Loop through strategies
    //         std::cout << i + 1 << ". " << strategies[i]->getName() << std::endl;    // Display strategy name
    //     }
    // }
};
#endif // TRADINGBOT_H
