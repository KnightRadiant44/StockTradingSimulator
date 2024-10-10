#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QDebug>
#include <QThread>
#include <QTimer>
#include "TradingBot.h"
MainWindow::MainWindow(const QString &username, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), tradingBot(),
    simulationThread(nullptr), updateTimer(nullptr)
{
    ui->setupUi(this);
    ui->UserName->setText(username);

    connect(ui->StartSim, &QPushButton::clicked, this, &MainWindow::onConfirmButtonClicked);
    connect(ui->exitButton, &QPushButton::clicked, this, &MainWindow::on_exitButton_clicked);

    setWindowTitle("Trading Stock Simulation - " + username);
    ValuesSet();
}
// update window and update thread timer
MainWindow::~MainWindow()
{
    if (simulationThread) {
        simulationThread->quit();
        simulationThread->wait();
        delete simulationThread;
    }
    if (updateTimer) {
        updateTimer->stop();
        delete updateTimer;
    }
    delete ui;
}

void MainWindow::ValuesSet()
{
    ui->CurrentPriceNum->setNum(tradingBot.getCurrentPrice());
    ui->TotalCurrentValueNum->setNum(tradingBot.getTotalValue());
    ui->MarketEventString->setText(QString::fromStdString(tradingBot.getLastMarketEvent()));
    // Update UI elements with values from tradingBot
    ui->Bal_User->setNum(tradingBot.getBalance());
    ui->StockNum->setNum(tradingBot.getOwnedStocks());
    ui->CumReturnNum->setNum(tradingBot.getTotalReturn() * 100);
    ui->MaxDrawDownNum->setNum(tradingBot.getMaxDrawdown() * 100);
    ui->VolatilityNum->setNum(tradingBot.getVolatility() * 100);
    ui->MarketVolatilityNum->setNum(tradingBot.getMarketVolatility() * 100);
    ui->SharpeRatioNum->setNum(tradingBot.getSharpeRatio());
    ui->CurrentStrategyNum->setText(QString::fromStdString(tradingBot.getCurrentStrategyName()));
    ui->CurrentPriceNum->setNum(tradingBot.getCurrentPrice());
    ui->CurrentBalanceNum->setNum(tradingBot.getBalance());
    ui->TotalCurrentValueNum->setNum(tradingBot.getTotalValue());
    ui->MarketEventString->setText(QString::fromStdString(tradingBot.getLastMarketEvent()));
    ui->PNLnum->setNum(tradingBot.getProfitLossSinceStart());
    ui->PNLnumLastUpdate->setNum(tradingBot.getProfitLossSinceLastUpdate());
    ui->DayNum->setNum(tradingBot.getCurrentDay());

    QString LastUpdateText = QString("Buys: %1, Sells: %2, Holds: %3")
                                 .arg(tradingBot.getLastBuys())
                                 .arg(tradingBot.getLastSells())
                                 .arg(tradingBot.getLastHolds());
    ui->ActionsSinceLastUpdateNum->setText(LastUpdateText);

    QString ActionsInfo = QString("Buys: %1, Sells: %2, Holds: %3")
                              .arg(tradingBot.getTotalBuys())
                              .arg(tradingBot.getTotalSells())
                              .arg(tradingBot.getTotalHolds());
    ui->TotalActionsNum->setText(ActionsInfo);
}

void MainWindow::onConfirmButtonClicked()
{
    // Get strategy selection
    int strategyIndex = -1;
    if (ui->BuyAndHold->isChecked()) strategyIndex = 0;
    else if (ui->MeanReversion->isChecked()) strategyIndex = 1;
    else if (ui->MovingAvg->isChecked()) strategyIndex = 2;
    else if (ui->TrendFollowing->isChecked()) strategyIndex = 3;
    else if (ui->Random->isChecked()) strategyIndex = 4;

    if (strategyIndex == -1) {
        QMessageBox::warning(this, "No Strategy Selection", "Please select a strategy before confirming.");
        return;
    }
    tradingBot.setStrategy(strategyIndex);

    // Get number of days
    int days = 0;
    if (ui->Day50->isChecked()) days = 50;
    else if (ui->Day100->isChecked()) days = 100;
    else if (ui->Day150->isChecked()) days = 150;
    else if (ui->Day200->isChecked()) days = 200;
    else if (ui->Day400->isChecked()) days = 400;
    else if (ui->Day800->isChecked()) days = 800;
    else if (ui->Day1000->isChecked()) days = 1000;
    else if (ui->Day2000->isChecked()) days = 2000;

    if (days == 0) {
        QMessageBox::warning(this, "No Day Selection", "Please select a number of days before confirming.");
        return;
    }

    // Set up and start the simulation
    tradingBot.setStrategy(strategyIndex);

    // Create and start the update timer
    if (!updateTimer) {
        updateTimer = new QTimer(this);
        connect(updateTimer, &QTimer::timeout, this, &MainWindow::updateUIFromBot);
    }
    updateTimer->start(1000); // Update every second

    // Create and start the simulation thread
    if (simulationThread) {
        simulationThread->quit();
        simulationThread->wait();
        delete simulationThread;
    }

    simulationThread = new QThread(this);

    // Move the trading bot to the new thread
    tradingBot.moveToThread(simulationThread);

    // Connect signals and slots for thread management
    connect(simulationThread, &QThread::started, [this, days]() {
        tradingBot.executeStrategy(days);
        simulationThread->quit();
    });
    connect(simulationThread, &QThread::finished, updateTimer, &QTimer::stop);

    simulationThread->start();
}

void MainWindow::updateUIFromBot()
{
    ValuesSet();

    // Update trades taken text box
    QFile file("/Users/shxhid01/Documents/Qt apps/Trading copy/trades_taken.txt");
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream in(&file);
        ui->TradesTakenBox->setText(in.readAll());
        file.close();
    }
}

// exit button to close window
void MainWindow::on_exitButton_clicked()
{
    QApplication::quit();
}
