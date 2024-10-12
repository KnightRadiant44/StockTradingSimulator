#ifndef TRADINGBOT_H
#define TRADINGBOT_H

#include <QObject>
#include <string>

class TradingStrategy;
class RiskManagement;
class Market;

class TradingBot : public QObject {
    Q_OBJECT

public:
    TradingBot(QObject *parent = nullptr);
    ~TradingBot();

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

signals:
    void updateUI();
    void simulationComplete();

private:
    class TradingBotImpl;
    TradingBotImpl* pImpl;
};

#endif // TRADINGBOT_H
