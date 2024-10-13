// mainwindow.h
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "TradingBot.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(const QString &username, QWidget *parent = nullptr);
    ~MainWindow();
    void on_exitButton_clicked();
    void on_HelpButton_clicked();

private slots:
    void onConfirmButtonClicked();
    void executeNextTradingDay();
    void updateUIFromBot();
    void onSimulationComplete();
    void OnResetButtonClicked();

private:
    Ui::MainWindow *ui;
    TradingBot *tradingBot;
    QTimer *updateTimer;

    void ValuesSet();
    int getSelectedStrategyIndex() const;
    int getSelectedDays() const;
    // Member variables
    int balance;
    int StocksOwned;
    int DayNum;
    int CurrentPrice;
    int CurrentBalance;
    int TotalCurrenValue;
    double Volatility;
    double CumulativeReturn;
    double MaximumDrawDown;
    double ShapreRatio;
    double MarketVolatility;
    std::string CurrentStrategy;
    std::string MarketEvent;

    // Actions
    int Buys, Sells, Hold;
    int LastBuys, LastSells, LastHolds;
    int ProfitLossSinceStart, ProfitLossSinceLastUpdate;
    int NumDay;
    int strategyChoice;
    int NumOfDays;
    int NumOfUpdates;

};

#endif // MAINWINDOW_H
