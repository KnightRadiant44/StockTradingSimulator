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
#include <QMessageBox>
#include <QDir>
#include <QtCharts/QtCharts>
#include <QApplication>
#include <QMainWindow>
#include <QtCharts>

class TradingBot::TradingBotImpl {
public:
    TradingBotImpl(const QString &username) :
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
        slippageFactor(0.001),
        username(username)
    {
        // Constructor body
        strategies.push_back(new BuyAndHoldStrategy());
        strategies.push_back(new MeanReversionStrategy());
        strategies.push_back(new TrendFollowingStrategy());
        strategies.push_back(new RandomStrategy());
        strategies.push_back(new MovingAverageStrategy());

        QString documentsPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
        tradesFilePath = documentsPath + "/TradingSimulation/" + username + "_trades_taken.txt";
    }

    ~TradingBotImpl() {
        for (auto strategy : strategies) {
            delete strategy;
        }
        delete riskManager;
        saveState();  // Save state when the bot is destroyed
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
        std::vector<std::string> events = {
            "Major product announcement",
            "Market downturn indicators",
            "Bull market predictions",
            "Uncertainty due to policy changes",
            "Competitor's actions impacting stock"
        };
        return events[std::rand() % events.size()];
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
    void saveState() {
        QString filePath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) +
                           "/TradingSimulation/" + username + "_saved_state.txt";

        QFile file(filePath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            // Save all relevant data
            out << "Balance: " << balance << "\n";
            out << "OwnedStocks: " << ownedStocks << "\n";
            out << "TotalBuys: " << totalBuys << "\n";
            out << "TotalSells: " << totalSells << "\n";
            out << "CurrentDay: " << currentDay << "\n";
            out << "MaxDrawdown: " << maxDrawdown << "\n";
            out << "TotalReturn: " << totalReturn << "\n";
            out << "Volatility: " << volatility << "\n";
            out << "SharpeRatio: " << sharpeRatio << "\n";
            out << "BuysSinceLastUpdate: " << buysSinceLastUpdate << "\n";
            out << "SellsSinceLastUpdate: " << sellsSinceLastUpdate << "\n";
            out << "HoldsSinceLastUpdate: " << holdsSinceLastUpdate << "\n";
            file.close();
        } else {
            QMessageBox::warning(nullptr, "File Error", "Unable to save state.");
        }
    }

    void loadState() {
        QString filePath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) +
                           "/TradingSimulation/" + username + "_saved_state.txt";

        QFile file(filePath);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            QString line;
            while (in.readLineInto(&line)) {
                QStringList parts = line.split(": ");
                if (parts.size() == 2) {
                    if (parts[0] == "Balance") {
                        balance = parts[1].toDouble();
                    } else if (parts[0] == "OwnedStocks") {
                        ownedStocks = parts[1].toInt();
                    } else if (parts[0] == "TotalBuys") {
                        totalBuys = parts[1].toInt();
                    } else if (parts[0] == "TotalSells") {
                        totalSells = parts[1].toInt();
                    } else if (parts[0] == "CurrentDay") {
                        currentDay = parts[1].toInt();
                    } else if (parts[0] == "MaxDrawdown") {
                        maxDrawdown = parts[1].toDouble();
                    } else if (parts[0] == "TotalReturn") {
                        totalReturn = parts[1].toDouble();
                    } else if (parts[0] == "Volatility") {
                        volatility = parts[1].toDouble();
                    } else if (parts[0] == "SharpeRatio") {
                        sharpeRatio = parts[1].toDouble();
                    } else if (parts[0] == "BuysSinceLastUpdate") {
                        buysSinceLastUpdate = parts[1].toInt();
                    } else if (parts[0] == "SellsSinceLastUpdate") {
                        sellsSinceLastUpdate = parts[1].toInt();
                    } else if (parts[0] == "HoldsSinceLastUpdate") {
                        holdsSinceLastUpdate = parts[1].toInt();
                    }
                }
            }
            file.close();
        } else {
            // Initialize to default values if the file doesn't exist
            resetToInitialState();
        }
    }

    void resetToInitialState() {
        balance = 10000.0;
        ownedStocks = 0;
        totalReturn = 0.0;
        maxDrawdown = 0.0;
        volatility = 0.0;
        sharpeRatio = 0.0;
        lastUpdateValue = balance;
        lastUpdateDay = 0;
        buysSinceLastUpdate = 0;
        sellsSinceLastUpdate = 0;
        holdsSinceLastUpdate = 0;
        totalBuys = 0;
        totalSells = 0;
        totalHolds = 0;
        currentDay = 0;
        balanceHistory.clear();
        balanceHistory.push_back(balance);
        stockPriceHistory.clear();
        stockPriceHistory.push_back(market.getPrice());
    }

    QString username;
    std::vector<double> stockPriceHistory;
    };




// Constructor
    TradingBot::TradingBot(const QString &username, QObject *parent)
        : QObject(parent), pImpl(new TradingBotImpl(username)) {
        pImpl->loadState();
        QString documentsPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
        QString tradingSimFolder = documentsPath + "/TradingSimulation";
        QDir().mkpath(tradingSimFolder);
        QString tradeFilePath = tradingSimFolder + "/" + username + "_trades_taken.txt";

        tradeFile.open(tradeFilePath.toStdString(), std::ios::out | std::ios::app);
        if (!tradeFile.is_open()) {
            QMessageBox::warning(nullptr, "Error", "Couldn't open trades_taken.txt file. Please check file permissions.");
        }
    }

TradingBot::~TradingBot() {
    if (tradeFile.is_open()) {
        tradeFile.close();
    }
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

    tradeFile.close();
    tradeFile.open(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation).toStdString() + "/TradingSimulation/trades_taken.txt", std::ios::out | std::ios::trunc);
    if (!tradeFile.is_open()) {
        QMessageBox::warning(nullptr, "Error", "Couldn't open trades_taken.txt file. Please check file permissions.");
    }
}

void TradingBot::executeNextDay() {
    // Update the day and generate market event
    double currentPrice = pImpl->market.getPrice();
    double totalValue = pImpl->balance + pImpl->ownedStocks * currentPrice;
    pImpl->balanceHistory.push_back(totalValue);
    pImpl->stockPriceHistory.push_back(currentPrice);
    pImpl->currentDay++;
    pImpl->lastMarketEvent = pImpl->generateMarketEvent();
    // Adjust market conditions and risk based on the event
    pImpl->market.adjustVolatility(pImpl->lastMarketEvent);
    pImpl->market.adjustOverallTrend(pImpl->lastMarketEvent);
    pImpl->riskManager->adjustRiskTolerance(pImpl->lastMarketEvent);

    // Execute trading strategy
    double action = pImpl->currentStrategy->execute(currentPrice, pImpl->lastMarketEvent);
    double adjustedAction = pImpl->riskManager->adjustPosition(action, currentPrice, pImpl->balance);
    currentPrice = pImpl->applySlippage(currentPrice);

    // Determine trade action based on adjusted action
    std::string tradeAction;
    double actionTaken = 0;
    if (adjustedAction > 0) { // Buy
        int stocksToBuy = static_cast<int>(adjustedAction * pImpl->balance / currentPrice);
        double cost = pImpl->applyTransactionFee(stocksToBuy * currentPrice);
        if (pImpl->balance >= cost) {
            pImpl->ownedStocks += stocksToBuy;
            pImpl->balance -= cost;
            tradeAction = "Buy";
            actionTaken = stocksToBuy;
            pImpl->buysSinceLastUpdate++;
        } else {
            tradeAction = "Hold (Insufficient funds)";
            pImpl->holdsSinceLastUpdate++;
        }
    } else if (adjustedAction < 0 && pImpl->ownedStocks > 0) { // Sell
        int stocksToSell = std::min(static_cast<int>(-adjustedAction * pImpl->ownedStocks), pImpl->ownedStocks);
        double revenue = pImpl->applyTransactionFee(stocksToSell * currentPrice);
        pImpl->ownedStocks -= stocksToSell;
        pImpl->balance += revenue;
        tradeAction = "Sell";
        actionTaken = stocksToSell;
        pImpl->sellsSinceLastUpdate++;
    } else { // Hold
        tradeAction = "Hold";
        pImpl->holdsSinceLastUpdate++;
    }

    // Log trade and save the state
    pImpl->logTrade(pImpl->currentDay, currentPrice, tradeAction, actionTaken);
    pImpl->saveState();

    // Update statistics and balance history
    pImpl->balanceHistory.push_back(totalValue);
    // pImpl->stockPriceHistory.push_back(currentPrice);

    pImpl->lastUpdateValue = totalValue;
    pImpl->lastUpdateDay = pImpl->currentDay;

    pImpl->maxDrawdown = pImpl->calculateMaxDrawdown();
    pImpl->totalReturn = pImpl->calculateCumulativeReturn();
    pImpl->volatility = pImpl->calculateVolatility();
    pImpl->sharpeRatio = pImpl->calculateSharpeRatio();

    // Emit update UI signal
    emit updateUI();

    // Check if simulation is complete and emit signal
    if (isSimulationComplete()) {
        emit simulationComplete();
    }
}



bool TradingBot::isSimulationComplete() const {
    return pImpl->currentDay >= pImpl->totalDays;
}
void TradingBot::logTrade(int day, double currentPrice, const std::string& tradeAction, double actionTaken) {
    if (tradeFile.is_open()) {
        double totalValue = pImpl->balance + pImpl->ownedStocks * currentPrice;
        tradeFile << "Day " << std::setw(3) << day
                  << ": Price = $" << std::fixed << std::setprecision(2) << currentPrice
                  << ", Action = " << std::setw(4) << tradeAction << " " << std::setw(3) << actionTaken
                  << " stocks, Balance = $" << std::setw(10) << pImpl->balance
                  << ", Owned Stocks = " << std::setw(4) << pImpl->ownedStocks
                  << ", Total Value = $" << std::setw(10) << totalValue << "\n";
        tradeFile.flush();  // Ensure the data is written immediately
    }
};


void TradingBot::resetToInitialState() {
    // Reset the trading bot's state to its initial values

    balanceHistory.clear();
    stockPriceHistory.clear();

    balanceHistory.clear();
    stockPriceHistory.clear();

    pImpl->balance = 10000.0;  // Initial balance
    pImpl->ownedStocks = 0;  // Reset owned stocks
    pImpl->totalReturn = 0.0;  // Reset total return
    pImpl->maxDrawdown = 0.0;  // Reset max drawdown
    pImpl->volatility = 0.0;  // Reset volatility
    pImpl->sharpeRatio = 0.0;  // Reset Sharpe ratio
    pImpl->lastUpdateValue = pImpl->balance;
    pImpl->lastUpdateDay = 0;
    pImpl->buysSinceLastUpdate = 0;
    pImpl->sellsSinceLastUpdate = 0;
    pImpl->holdsSinceLastUpdate = 0;
    pImpl->totalBuys = 0;
    pImpl->totalSells = 0;
    pImpl->totalHolds = 0;
    pImpl->currentDay = 0;
    pImpl->balanceHistory.clear();
    pImpl->balanceHistory.push_back(pImpl->balance);
    pImpl->saveState();
    pImpl->resetToInitialState();
    pImpl->saveState();
}


void TradingBot::generateBalanceGraph() {
    QLineSeries *series = new QLineSeries();

    for (size_t i = 0; i < pImpl->balanceHistory.size(); ++i) {
        series->append(i, pImpl->balanceHistory[i]);
    }

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->createDefaultAxes();
    chart->setTitle("Balance History");
    chart->setBackgroundBrush(QBrush(Qt::white));

    // Set text color to black for better visibility on white background
    chart->setTitleBrush(QBrush(Qt::black));
    chart->axisX()->setLabelsBrush(QBrush(Qt::black));
    chart->axisY()->setLabelsBrush(QBrush(Qt::black));

    // Color the line based on trend
    QLinearGradient gradient(QPointF(0, 0), QPointF(1, 1));
    for (size_t i = 1; i < pImpl->balanceHistory.size(); ++i) {
        qreal pos = i / qreal(pImpl->balanceHistory.size() - 1);
        if (pImpl->balanceHistory[i] >= pImpl->balanceHistory[i-1]) {
            gradient.setColorAt(pos, Qt::darkGreen);
        } else {
            gradient.setColorAt(pos, Qt::darkRed);
        }
    }
    series->setBrush(gradient);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->resize(800, 600);

    // Create TradingSimulation folder in Documents if it doesn't exist
    QString documentsPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QString folderPath = documentsPath + "/TradingSimulation";
    QDir().mkpath(folderPath);

    // Save as PNG in the TradingSimulation folder
    QPixmap pixmap = chartView->grab();
    pixmap.save(folderPath + "/" + pImpl->username + "_balance_history.png");

    delete chartView;
}

void TradingBot::generateStockPriceGraph() {
    QLineSeries *series = new QLineSeries();

    for (size_t i = 0; i < pImpl->stockPriceHistory.size(); ++i) {
        series->append(i, pImpl->stockPriceHistory[i]);
    }

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->createDefaultAxes();
    chart->setTitle("Stock Price History");
    chart->setBackgroundBrush(QBrush(Qt::white));

    // Set text color to black for better visibility on white background
    chart->setTitleBrush(QBrush(Qt::black));
    chart->axisX()->setLabelsBrush(QBrush(Qt::black));
    chart->axisY()->setLabelsBrush(QBrush(Qt::black));

    // Color the line based on trend
    QLinearGradient gradient(QPointF(0, 0), QPointF(1, 1));
    for (size_t i = 1; i < pImpl->stockPriceHistory.size(); ++i) {
        qreal pos = i / qreal(pImpl->stockPriceHistory.size() - 1);
        if (pImpl->stockPriceHistory[i] >= pImpl->stockPriceHistory[i-1]) {
            gradient.setColorAt(pos, Qt::darkGreen);
        } else {
            gradient.setColorAt(pos, Qt::darkRed);
        }
    }
    series->setBrush(gradient);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->resize(800, 600);

    // Create TradingSimulation folder in Documents if it doesn't exist
    QString documentsPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QString folderPath = documentsPath + "/TradingSimulation";
    QDir().mkpath(folderPath);

    // Save as PNG in the TradingSimulation folder
    QPixmap pixmap = chartView->grab();
    pixmap.save(folderPath + "/" + pImpl->username + "_stock_price_history.png");

    delete chartView;
}

void TradingBot::generateGraphs() {
    generateBalanceGraph();
    generateStockPriceGraph();
}

