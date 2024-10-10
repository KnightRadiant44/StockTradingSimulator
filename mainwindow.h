// mainwindow.h
#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "TradingBot.h"
#include <QMainWindow>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(const QString &username, QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_exitButton_clicked();
    void onConfirmButtonClicked();
    void updateUIFromBot(); // Add this line


private:
    Ui::MainWindow *ui;
    QThread* simulationThread;
    QTimer* updateTimer;
    TradingBot tradingBot;

    void ValuesSet();

    // Member variables
    double balance;
    int StocksOwned;
    int DayNum;
    double CurrentPrice;
    double CurrentBalance;
    double TotalCurrenValue;
    double Volatility;
    double CumulativeReturn;
    double MaximumDrawDown;
    double ShapreRatio;
    double MarketVolatility;
    std::string CurrentStrategy;
    std::string MarketEvent;
    int Buys, Sells, Hold;
    int LastBuys, LastSells, LastHolds;
    double ProfitLossSinceStart;
    double ProfitLossSinceLastUpdate;
    int NumDay;
    int strategyChoice;
    int NumOfDays;
    int NumOfUpdates;
};
#endif // MAINWINDOW_H
