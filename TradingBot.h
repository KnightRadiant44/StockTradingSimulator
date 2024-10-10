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
class TradingBot : public QObject { // Inherit from QObject
    Q_OBJECT // Necessary for QObject-derived classes
private:
    // Add this near the top of your private members
    std::string lastMarketEvent;

    // Update your existing generateMarketEvent function
    std::string generateMarketEvent() {
        std::vector<std::string> events = {
            "Major product announcement",
            "Market downturn indicators",
            "Bull market predictions",
            "Uncertainty due to policy changes",
            "Competitor's actions impacting stock"
        };
        lastMarketEvent = events[std::rand() % events.size()];
        return lastMarketEvent;
    }    std::vector<TradingStrategy*> strategies;
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
    int Day;
    int Days;

    const double transactionFeePercentage = 0.005; // 0.5% fee per transaction
    const double slippageFactor = 0.001; // 0.1% slippage on each trade

    double maxDrawdown;
    double totalReturn;
    double volatility;
    double sharpeRatio;


public:
    // Update these getter methods
    double getCurrentPrice() const { return market.getPrice(); }
    double getTotalValue() const { return balance + ownedStocks * market.getPrice(); }
    const std::string& getLastMarketEvent() const { return lastMarketEvent; }

    // Make sure other getter methods are const-correct
    double getBalance() const { return balance; }
    int getOwnedStocks() const { return ownedStocks; }
    double getTotalReturn() const { return totalReturn; }
    double getMaxDrawdown() const { return maxDrawdown; }
    double getVolatility() const { return volatility; }
    double getMarketVolatility() const { return market.getVolatility(); }
    double getSharpeRatio() const { return sharpeRatio; }
    std::string getCurrentStrategyName() const {
        return currentStrategy ? currentStrategy->getName() : "None";
    }
    double getProfitLossSinceStart() const { return getTotalValue() - initialTotalValue; }
    double getProfitLossSinceLastUpdate() const { return getTotalValue() - lastUpdateValue; }
    int getCurrentDay() const { return Day; }
    int getLastBuys() const { return buysSinceLastUpdate; }
    int getLastSells() const { return sellsSinceLastUpdate; }
    int getLastHolds() const { return holdsSinceLastUpdate; }
    int getTotalBuys() const { return totalBuys; }
    int getTotalSells() const { return totalSells; }
    int getTotalHolds() const { return totalHolds; }

    void displayUpdate(int day, double currentPrice, double totalValue, const std::string& marketEvent) {
        std::cout << "\n--- Update for Day " << day << " ---" << std::endl;
        std::cout << "Current price: $" << std::fixed << std::setprecision(2) << currentPrice << std::endl;
        std::cout << "Current balance: $" << balance << std::endl;
        std::cout << "Owned stocks: " << ownedStocks << " (valued at $" << ownedStocks * currentPrice << ")" << std::endl;
        std::cout << "Total current value: $" << totalValue << std::endl;

        std::cout << "Actions since last update: " << buysSinceLastUpdate << " buys, "
                  << sellsSinceLastUpdate << " sells, " << holdsSinceLastUpdate << " holds" << std::endl;
        std::cout << "Total actions: " << totalBuys << " buys, "
                  << totalSells << " sells, " << totalHolds << " holds" << std::endl;

        if (lastUpdateDay > 0) {
            double profitSinceLastUpdate = totalValue - lastUpdateValue;
            std::cout << "Profit/Loss since last update: $" << profitSinceLastUpdate
                      << " (" << (profitSinceLastUpdate / lastUpdateValue * 100) << "%)" << std::endl;
        }

        double profitSinceStart = totalValue - initialTotalValue;
        std::cout << "Profit/Loss since start: $" << profitSinceStart
                  << " (" << (profitSinceStart / initialTotalValue * 100) << "%)" << std::endl;

        std::cout << "Current strategy: " << currentStrategy->getName() << std::endl;
        std::cout << "Market volatility: " << market.getVolatility() * 100 << "%" << std::endl;
        std::cout << "Market event: " << marketEvent << std::endl;

        lastUpdateValue = totalValue;
        lastUpdateDay = day;
    }

    double applyTransactionFee(double amount) {
        return amount * (1.0 - transactionFeePercentage); // Deduct fee
    }

    // Apply slippage to the execution price
    double applySlippage(double currentPrice) {
        double slippage = currentPrice * slippageFactor * ((std::rand() % 2 == 0) ? 1 : -1); // Random slippage
        return currentPrice + slippage;
    }

    // Calculate maximum drawdown
    double calculateMaxDrawdown() {
        double peak = balanceHistory[0];
        double maxDrawdown = 0;
        for (double value : balanceHistory) {
            peak = std::max(peak, value);
            double drawdown = (peak - value) / peak;
            maxDrawdown = std::max(maxDrawdown, drawdown);
        }
        return maxDrawdown;
    }

    // Calculate cumulative return
    double calculateCumulativeReturn() {
        double finalValue = balanceHistory.back();
        return (finalValue - initialTotalValue) / initialTotalValue;
    }

    // Calculate volatility (standard deviation of returns)
    double calculateVolatility() {
        std::vector<double> returns;
        for (size_t i = 1; i < balanceHistory.size(); ++i) {
            double dailyReturn = (balanceHistory[i] - balanceHistory[i - 1]) / balanceHistory[i - 1];
            returns.push_back(dailyReturn);
        }
        double meanReturn = std::accumulate(returns.begin(), returns.end(), 0.0) / returns.size();
        double variance = 0;
        for (double r : returns) {
            variance += std::pow(r - meanReturn, 2);
        }
        variance /= returns.size();
        return std::sqrt(variance); // Standard deviation (volatility)
    }

    // Calculate Sharpe Ratio
    double calculateSharpeRatio(double riskFreeRate = 0.0) {
        double meanReturn = calculateCumulativeReturn() / balanceHistory.size(); // Mean return
        return (meanReturn - riskFreeRate) / volatility;
    }

public:
    TradingBot() : currentStrategy(nullptr), balance(10000), ownedStocks(0),
        riskManager(new RiskManagement(0.02, 0.1)),
        lastUpdateValue(10000), lastUpdateDay(0), initialTotalValue(10000),
        buysSinceLastUpdate(0), sellsSinceLastUpdate(0), holdsSinceLastUpdate(0),
        totalBuys(0), totalSells(0), totalHolds(0),
        maxDrawdown(0), totalReturn(0), volatility(0), sharpeRatio(0) {
        strategies.push_back(new BuyAndHoldStrategy());
        strategies.push_back(new MeanReversionStrategy());
        strategies.push_back(new TrendFollowingStrategy());
        strategies.push_back(new RandomStrategy());
        strategies.push_back(new MovingAverageStrategy());
    }

    ~TradingBot() {
        for (auto strategy : strategies) {
            delete strategy;
        }
        delete riskManager;
    }

    void setStrategy(int index) {
        if (index >= 0 && index < strategies.size()) {
            currentStrategy = strategies[index];
        }
    }

    void executeStrategy(int days) {
        if (!currentStrategy) {
            std::cout << "Please select a strategy first." << std::endl;
            return;
        }

        initialTotalValue = balance + ownedStocks * market.getPrice();
        std::cout << "Starting balance: $" << balance << std::endl;
        std::cout << "Starting owned stocks: " << ownedStocks << " (valued at $" << ownedStocks * market.getPrice() << ")" << std::endl;
        std::cout << "Total starting value: $" << initialTotalValue << std::endl;
        std::cout << "Using strategy: " << currentStrategy->getName() << std::endl;

        std::cout << "\nHow often do you want to see updates? (Enter number of days): ";
        int updateFrequency;
        std::cin >> updateFrequency;
        std::cin.ignore();

        balanceHistory.clear();
        balanceHistory.push_back(initialTotalValue);
        std::ofstream dataFile("balance_history.txt");
        std::ofstream tradesFile("trades_taken.txt");

        buysSinceLastUpdate = sellsSinceLastUpdate = holdsSinceLastUpdate = 0;
        totalBuys = totalSells = totalHolds = 0;

        for (int day = 1; day <= days; ++day) {
            double currentPrice = market.getPrice();
            std::string marketEvent = generateMarketEvent();

            market.adjustVolatility(marketEvent);
            market.adjustOverallTrend(marketEvent);
            riskManager->adjustRiskTolerance(marketEvent);

            double action = currentStrategy->execute(currentPrice, marketEvent);
            double adjustedAction = riskManager->adjustPosition(action, currentPrice, balance);

            currentPrice = applySlippage(currentPrice);

            std::string tradeAction;
            double actionTaken = 0;

            if (adjustedAction > 0) { // Buying
                int stocksToBuy = static_cast<int>(adjustedAction * balance / currentPrice);
                double cost = applyTransactionFee(stocksToBuy * currentPrice); // Apply transaction fee
                if (balance >= cost) {
                    ownedStocks += stocksToBuy;
                    balance -= cost;
                    tradeAction = "Buy";
                    actionTaken = stocksToBuy;
                    buysSinceLastUpdate++;
                    totalBuys++;
                } else {
                    tradeAction = "Hold (Insufficient funds)";
                    holdsSinceLastUpdate++;
                    totalHolds++;
                }
            } else if (adjustedAction < 0 && ownedStocks > 0) { // Selling
                int stocksToSell = std::min(static_cast<int>(-adjustedAction * ownedStocks), ownedStocks);
                double revenue = applyTransactionFee(stocksToSell * currentPrice); // Apply transaction fee
                ownedStocks -= stocksToSell;
                balance += revenue;
                tradeAction = "Sell";
                actionTaken = stocksToSell;
                sellsSinceLastUpdate++;
                totalSells++;
            } else {
                tradeAction = "Hold";
                holdsSinceLastUpdate++;
                totalHolds++;
            }

            double totalValue = balance + ownedStocks * currentPrice;
            balanceHistory.push_back(totalValue);

            tradesFile << "Day " << day << ": Price = $" << currentPrice
                       << ", Action = " << tradeAction << " " << actionTaken
                       << " stocks, Balance = $" << balance
                       << ", Owned Stocks = " << ownedStocks << "\n";

            dataFile << day << " " << totalValue << std::endl;

            if (day % updateFrequency == 0 || day == days) {
                displayUpdate(day, currentPrice, totalValue, marketEvent);

                if (day < days) {
                    std::cout << "\nDo you want to change the strategy? (y/n): ";
                    char choice;
                    std::cin >> choice;
                    std::cin.ignore();

                    if (choice == 'y' || choice == 'Y') {
                        displayStrategies();
                        std::cout << "Select new strategy (1-5): ";
                        int strategyChoice;
                        std::cin >> strategyChoice;
                        setStrategy(strategyChoice - 1);
                        std::cout << "Strategy changed to: " << currentStrategy->getName() << std::endl;
                    }

                    std::cout << "\nPress Enter to continue the simulation.";
                    std::cin.get();
                }

                buysSinceLastUpdate = sellsSinceLastUpdate = holdsSinceLastUpdate = 0;
            }
        }

        tradesFile.close();
        dataFile.close();

        double finalValue = balance + ownedStocks * market.getPrice();
        double profit = finalValue - initialTotalValue;
        std::cout << "\nSimulation complete!" << std::endl;
        std::cout << "Final balance: $" << balance << std::endl;
        std::cout << "Owned stocks: " << ownedStocks << " (valued at $" << ownedStocks * market.getPrice() << ")" << std::endl;
        std::cout << "Total final value: $" << finalValue << std::endl;
        std::cout << "Total " << (profit >= 0 ? "profit" : "loss") << ": $" << std::abs(profit) << std::endl;

        // Calculate and display performance metrics
        maxDrawdown = calculateMaxDrawdown();
        totalReturn = calculateCumulativeReturn();
        volatility = calculateVolatility();
        sharpeRatio = calculateSharpeRatio();

        std::cout << "\nPerformance Metrics:" << std::endl;
        std::cout << "Cumulative Return: " << totalReturn * 100 << "%" << std::endl;
        std::cout << "Maximum Drawdown: " << maxDrawdown * 100 << "%" << std::endl;
        std::cout << "Volatility: " << volatility * 100 << "%" << std::endl;
        std::cout << "Sharpe Ratio: " << sharpeRatio << std::endl;

        system("gnuplot -p -e \"plot 'balance_history.txt' using 1:2 with lines title 'Total Value Over Days';\"");
        std::cout << "Balance chart displayed in window." << std::endl;
        std::cout << "Detailed trades saved in 'trades_taken.txt'." << std::endl;
    }

    void displayStrategies() {
        std::cout << "Available strategies:" << std::endl;
        for (int i = 0; i < strategies.size(); ++i) {
            std::cout << i + 1 << ". " << strategies[i]->getName() << std::endl;
        }
    }
};

#endif // TRADINGBOT_H
