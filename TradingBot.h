#ifndef TRADINGBOT_H
#define TRADINGBOT_H

#include <QObject>
#include <string>
#include <fstream>
#include <vector>
#include <QtCharts>
#include <QString>
#include <string>
#include <vector>
class TradingStrategy;
class RiskManagement;
class Market;

class TradingBot : public QObject {
    Q_OBJECT

public:
    explicit TradingBot(const QString &username, QObject *parent = nullptr);
    ~TradingBot();
    void recordTrade(const std::string& action, double price, int quantity);
    void resetToInitialState();
    QString username;

    // Getters
    double getCurrentPrice() const;
    double getTotalValue() const;
    const std::string& getLastMarketEvent() const;
    double getBalance() const;
    int getOwnedStocks() const;
    double getTotalReturn() const;
    double getMaxDrawdown() const;
    double getVolatility() const;
    double getMarketVolatility() const;
    double getSharpeRatio() const;
    std::string getCurrentStrategyName() const;
    double getProfitLossSinceStart() const;
    double getProfitLossSinceLastUpdate() const;
    int getCurrentDay() const;
    int getLastBuys() const;
    int getLastSells() const;
    int getLastHolds() const;
    int getTotalBuys() const;
    int getTotalSells() const;
    int getTotalHolds() const;

    // Setters and control functions
    void setStrategy(int index);
    void initializeSimulation(int days);
    void executeNextDay();
    bool isSimulationComplete() const;
    void updateTotalActions();

    // New functions for graph generation
    void generateBalanceGraph();
    void generateStockPriceGraph();
    void generateGraphs();


private:
    class TradingBotImpl;
    TradingBotImpl* pImpl;
    std::ofstream tradeFile;

    void logTrade(int day, double currentPrice, const std::string& tradeAction, double actionTaken);

    // New vectors to store balance and stock price history
    std::vector<double> balanceHistory;
    std::vector<double> stockPriceHistory;

signals:
    void updateUI();
    void simulationComplete();
};






#endif // TRADINGBOT_H
