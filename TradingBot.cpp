#include "TradingBot.h"
#include "TradingStrategy.h"
#include "RiskManagement.h"
#include "Market.h"
#include "BuyAndHold.h"
#include "MeanReversion.h"
#include "MovingAvg.h"
#include "RandomStrategy.h"
#include "TrendFollowing.h"
#include <QFile>
#include <QTextStream>
#include <QStandardPaths>
#include <cmath>
#include <numeric>
#include <algorithm>

class TradingBot::TradingBotImpl {
public:
    TradingBotImpl() :
        currentStrategy(nullptr),
        balance(10000),
        ownedStocks(0),
        riskManager(new RiskManagement(0.02, 0.1)),
        lastUpdateValue(10000),
        lastUpdateDay(0),
        initialTotalValue(10000),
        buysSinceLastUpdate(0),
        sellsSinceLastUpdate(0),
        holdsSinceLastUpdate(0),
        totalBuys(0),
        totalSells(0),
        totalHolds(0),
        maxDrawdown(0),
        totalReturn(0),
        volatility(0),
        sharpeRatio(0),
        currentDay(0),
        totalDays(0),
        transactionFeePercentage(0.005),
        slippageFactor(0.001)
    {
        strategies.push_back(new BuyAndHoldStrategy());
        strategies.push_back(new MeanReversionStrategy());
        strategies.push_back(new TrendFollowingStrategy());
        strategies.push_back(new RandomStrategy());
        strategies.push_back(new MovingAverageStrategy());

        QString documentsPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
        tradesFilePath = documentsPath + "/trades_taken.txt";
    }

    ~TradingBotImpl() {
        for (auto strategy : strategies) {
            delete strategy;
        }
        delete riskManager;
    }

    std::vector<TradingStrategy*> strategies;
    TradingStrategy* currentStrategy;
    double balance;
    int ownedStocks;
    std::vector<double> balanceHistory;
    Market market;
    RiskManagement* riskManager;
    QString tradesFilePath;
    std::string lastMarketEvent;
    double lastUpdateValue;
    int lastUpdateDay;
    double initialTotalValue;
    int buysSinceLastUpdate, sellsSinceLastUpdate, holdsSinceLastUpdate;
    int totalBuys, totalSells, totalHolds;
    int currentDay;
    int totalDays;
    double maxDrawdown;
    double totalReturn;
    double volatility;
    double sharpeRatio;
    const double transactionFeePercentage;
    const double slippageFactor;

    std::string generateMarketEvent() {
        // Implementation of generateMarketEvent
        // ...
        return "Normal market conditions"; // Placeholder
    }

    double applyTransactionFee(double amount) {
        return amount * (1.0 - transactionFeePercentage);
    }

    double applySlippage(double currentPrice) {
        double slippage = currentPrice * slippageFactor * ((std::rand() % 2 == 0) ? 1 : -1);
        return currentPrice + slippage;
    }

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

    double calculateCumulativeReturn() {
        double finalValue = balanceHistory.back();
        return (finalValue - initialTotalValue) / initialTotalValue;
    }

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
        return std::sqrt(variance);
    }

    double calculateSharpeRatio(double riskFreeRate = 0.0) {
        double meanReturn = calculateCumulativeReturn() / balanceHistory.size();
        return (meanReturn - riskFreeRate) / volatility;
    }

    void logTrade(int day, double currentPrice, const std::string& tradeAction, double actionTaken) {
        QFile file(tradesFilePath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream stream(&file);
            stream << "Day " << day << ": Price = $" << currentPrice
                   << ", Action = " << QString::fromStdString(tradeAction) << " " << actionTaken
                   << " stocks, Balance = $" << balance
                   << ", Owned Stocks = " << ownedStocks << "\n";
            file.close();
        }
    }
};

TradingBot::TradingBot(QObject *parent) : QObject(parent), pImpl(new TradingBotImpl()) {}

TradingBot::~TradingBot() {
    delete pImpl;
}

double TradingBot::getCurrentPrice() const { return pImpl->market.getPrice(); }
double TradingBot::getTotalValue() const { return pImpl->balance + pImpl->ownedStocks * pImpl->market.getPrice(); }
const std::string& TradingBot::getLastMarketEvent() const { return pImpl->lastMarketEvent; }
double TradingBot::getBalance() const { return pImpl->balance; }
int TradingBot::getOwnedStocks() const { return pImpl->ownedStocks; }
double TradingBot::getTotalReturn() const { return pImpl->totalReturn; }
double TradingBot::getMaxDrawdown() const { return pImpl->maxDrawdown; }
double TradingBot::getVolatility() const { return pImpl->volatility; }
double TradingBot::getMarketVolatility() const { return pImpl->market.getVolatility(); }
double TradingBot::getSharpeRatio() const { return pImpl->sharpeRatio; }
std::string TradingBot::getCurrentStrategyName() const { return pImpl->currentStrategy ? pImpl->currentStrategy->getName() : "No strategy set"; }
double TradingBot::getProfitLossSinceStart() const { return getTotalValue() - pImpl->initialTotalValue; }
double TradingBot::getProfitLossSinceLastUpdate() const { return getTotalValue() - pImpl->lastUpdateValue; }
int TradingBot::getCurrentDay() const { return pImpl->currentDay; }
int TradingBot::getLastBuys() const { return pImpl->buysSinceLastUpdate; }
int TradingBot::getLastSells() const { return pImpl->sellsSinceLastUpdate; }
int TradingBot::getLastHolds() const { return pImpl->holdsSinceLastUpdate; }
int TradingBot::getTotalBuys() const { return pImpl->totalBuys; }
int TradingBot::getTotalSells() const { return pImpl->totalSells; }
int TradingBot::getTotalHolds() const { return pImpl->totalHolds; }

void TradingBot::setStrategy(int index) {
    if (index >= 0 && index < pImpl->strategies.size()) {
        pImpl->currentStrategy = pImpl->strategies[index];
    }
}

void TradingBot::initializeSimulation(int days) {
    pImpl->totalDays = days;
    pImpl->currentDay = 0;
    pImpl->initialTotalValue = pImpl->balance + pImpl->ownedStocks * pImpl->market.getPrice();
    pImpl->balanceHistory.clear();
    pImpl->balanceHistory.push_back(pImpl->initialTotalValue);

    QFile file(pImpl->tradesFilePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        file.close();
    }

    pImpl->buysSinceLastUpdate = pImpl->sellsSinceLastUpdate = pImpl->holdsSinceLastUpdate = 0;
    pImpl->totalBuys = pImpl->totalSells = pImpl->totalHolds = 0;
}

void TradingBot::executeNextDay() {
    pImpl->currentDay++;
    double currentPrice = pImpl->market.getPrice();
    pImpl->lastMarketEvent = pImpl->generateMarketEvent();

    pImpl->market.adjustVolatility(pImpl->lastMarketEvent);
    pImpl->market.adjustOverallTrend(pImpl->lastMarketEvent);
    pImpl->riskManager->adjustRiskTolerance(pImpl->lastMarketEvent);

    double action = pImpl->currentStrategy->execute(currentPrice, pImpl->lastMarketEvent);
    double adjustedAction = pImpl->riskManager->adjustPosition(action, currentPrice, pImpl->balance);

    currentPrice = pImpl->applySlippage(currentPrice);

    std::string tradeAction;
    double actionTaken = 0;

    if (adjustedAction > 0) {
        int stocksToBuy = static_cast<int>(adjustedAction * pImpl->balance / currentPrice);
        double cost = pImpl->applyTransactionFee(stocksToBuy * currentPrice);
        if (pImpl->balance >= cost) {
            pImpl->ownedStocks += stocksToBuy;
            pImpl->balance -= cost;
            tradeAction = "Buy";
            actionTaken = stocksToBuy;
            pImpl->buysSinceLastUpdate++;
            pImpl->totalBuys++;
        } else {
            tradeAction = "Hold (Insufficient funds)";
            pImpl->holdsSinceLastUpdate++;
            pImpl->totalHolds++;
        }
    } else if (adjustedAction < 0 && pImpl->ownedStocks > 0) {
        int stocksToSell = std::min(static_cast<int>(-adjustedAction * pImpl->ownedStocks), pImpl->ownedStocks);
        double revenue = pImpl->applyTransactionFee(stocksToSell * currentPrice);
        pImpl->ownedStocks -= stocksToSell;
        pImpl->balance += revenue;
        tradeAction = "Sell";
        actionTaken = stocksToSell;
        pImpl->sellsSinceLastUpdate++;
        pImpl->totalSells++;
    } else {
        tradeAction = "Hold";
        pImpl->holdsSinceLastUpdate++;
        pImpl->totalHolds++;
    }

    double totalValue = pImpl->balance + pImpl->ownedStocks * currentPrice;
    pImpl->balanceHistory.push_back(totalValue);

    pImpl->logTrade(pImpl->currentDay, currentPrice, tradeAction, actionTaken);

    pImpl->lastUpdateValue = totalValue;
    pImpl->lastUpdateDay = pImpl->currentDay;

    pImpl->maxDrawdown = pImpl->calculateMaxDrawdown();
    pImpl->totalReturn = pImpl->calculateCumulativeReturn();
    pImpl->volatility = pImpl->calculateVolatility();
    pImpl->sharpeRatio = pImpl->calculateSharpeRatio();

    emit updateUI();

    if (isSimulationComplete()) {
        emit simulationComplete();
    }
}

bool TradingBot::isSimulationComplete() const {
    return pImpl->currentDay >= pImpl->totalDays;
}
