#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QDebug>
#include <QTimer>
#include "TradingBot.h"
#include <QStandardPaths>

// Constructor for the MainWindow class
MainWindow::MainWindow(const QString &username, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow),
    updateTimer(nullptr), tradingBot(nullptr)
{
    ui->setupUi(this); // Setup the UI elements
    ui->UserName->setText(username); // Set the username display

    // Create and connect the TradingBot
    tradingBot = new TradingBot(this);
    connect(tradingBot, &TradingBot::updateUI, this, &MainWindow::updateUIFromBot);
    connect(tradingBot, &TradingBot::simulationComplete, this, &MainWindow::onSimulationComplete);

    // Connect button signals to corresponding slots
    connect(ui->StartSim, &QPushButton::clicked, this, &MainWindow::onConfirmButtonClicked);
    connect(ui->exitButton, &QPushButton::clicked, this, &MainWindow::on_exitButton_clicked);
    connect(ui->HelpButton, &QPushButton::clicked, this, &MainWindow::on_HelpButton_clicked);

    // Set window title
    setWindowTitle("Trading Stock Simulation - " + username);

    // Set initial values for UI elements
    ValuesSet();
}


// Destructor for the MainWindow class
MainWindow::~MainWindow()
{
    // Stop and delete the update timer if it exists
    if (updateTimer) {
        updateTimer->stop(); // Stop the timer
        delete updateTimer;   // Delete the timer object
    }
    delete tradingBot; // Delete TradingBot object
    delete ui; // Delete UI object
}

// Function to update UI elements with values from the trading bot
void MainWindow::ValuesSet()
{
    // Update various UI components with current values from the trading bot
    ui->CurrentPriceNum->setNum(tradingBot->getCurrentPrice());
    ui->TotalCurrentValueNum->setNum(tradingBot->getTotalValue());
    ui->MarketEventString->setText(QString::fromStdString(tradingBot->getLastMarketEvent()));

    // Update additional UI elements with trading bot values
    ui->Bal_User->setNum(tradingBot->getBalance());
    ui->StockNum->setNum(tradingBot->getOwnedStocks());
    ui->CumReturnNum->setNum(tradingBot->getTotalReturn() * 100);
    ui->MaxDrawDownNum->setNum(tradingBot->getMaxDrawdown() * 100);
    ui->VolatilityNum->setNum(tradingBot->getVolatility() * 100);
    ui->MarketVolatilityNum->setNum(tradingBot->getMarketVolatility() * 100);
    ui->SharpeRatioNum->setNum(tradingBot->getSharpeRatio());
    ui->CurrentStrategyNum->setText(QString::fromStdString(tradingBot->getCurrentStrategyName()));

    ui->CurrentBalanceNum->setNum(tradingBot->getBalance());
    ui->PNLnum_2->setNum(tradingBot->getProfitLossSinceStart());
    ui->PNLnum->setNum(tradingBot->getProfitLossSinceStart());
    ui->PNLnumLastUpdate->setNum(tradingBot->getProfitLossSinceLastUpdate());
    ui->DayNum->setNum(tradingBot->getCurrentDay());

}

// Slot function triggered when the Start Simulation button is clicked
void MainWindow::onConfirmButtonClicked()
{
    int strategyIndex = getSelectedStrategyIndex();
    int days = getSelectedDays();

    // Check if a strategy has been selected
    if (strategyIndex == -1) {
        QMessageBox::warning(this, "No Strategy Selection", "Please select a strategy before confirming.");
        return;
    }

    // Check if a valid number of days has been selected
    if (days == 0) {
        QMessageBox::warning(this, "No Day Selection", "Please select a number of days before confirming.");
        return;
    }

    tradingBot->setStrategy(strategyIndex);
    tradingBot->initializeSimulation(days);

    // Initialize the update timer if it hasn't been already
    if (!updateTimer) {
        updateTimer = new QTimer(this);
        connect(updateTimer, &QTimer::timeout, this, &MainWindow::executeNextTradingDay);
    }
    updateTimer->start(10); // Execute a day every 0.01 seconds or 10 milliseconds
}

// Function to execute the next trading day
void MainWindow::executeNextTradingDay()
{
    if (!tradingBot->isSimulationComplete()) {
        tradingBot->executeNextDay();
        updateUIFromBot();
    } else {
        updateTimer->stop();
        onSimulationComplete();
    }
}

// Function to update values from the trading bot
void MainWindow::updateUIFromBot()
{
    ValuesSet(); // Refresh the UI with the latest values from the trading bot
}

// Slot function triggered when the simulation is complete
void MainWindow::onSimulationComplete()
{
    QMessageBox::information(this, "Simulation Complete", "The trading simulation has finished.");

    // Construct the path to trades_taken.txt dynamically
    QString homeDir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    QString tradesFilePath = homeDir + "/Documents/TradingSimulation/trades_taken.txt";

    // Read and display the contents of trades_taken.txt
    QFile tradesFile(tradesFilePath);
    if (tradesFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&tradesFile);
        QString tradesContent = in.readAll();
        ui->TradesTakenBox->setPlainText(tradesContent);
        tradesFile.close();
    } else {
        QMessageBox::warning(this, "File Error", "Unable to open trades_taken.txt");
    }
}

// Function to get the selected strategy index
int MainWindow::getSelectedStrategyIndex() const
{
    if (ui->BuyAndHold->isChecked()) return 0;
    if (ui->MeanReversion->isChecked()) return 1;
    if (ui->MovingAvg->isChecked()) return 2;
    if (ui->TrendFollowing->isChecked()) return 3;
    if (ui->Random->isChecked()) return 4;
    return -1; // No strategy selected
}

// Function to get the selected number of days
int MainWindow::getSelectedDays() const
{
    if (ui->Day50->isChecked()) return 50;
    if (ui->Day100->isChecked()) return 100;
    if (ui->Day150->isChecked()) return 150;
    if (ui->Day200->isChecked()) return 200;
    if (ui->Day400->isChecked()) return 400;
    if (ui->Day800->isChecked()) return 800;
    if (ui->Day1000->isChecked()) return 1000;
    if (ui->Day2000->isChecked()) return 2000;
    return 0; // No days selected
}

// Slot function triggered when the exit button is clicked
void MainWindow::on_exitButton_clicked()
{
    QApplication::quit(); // Close the application
}


void MainWindow::on_HelpButton_clicked()
{
    QString instructions = "Instructions on how to use the application:\n\n"
                           "1. Select a strategy to use on the top right.\n"
                           "2. Choose the number of days you want to simulate.\n"
                           "3. Enter how frequently you want updates to change strategies or not.\n"
                           "4. On the left, you can see your stats and trading information.\n"
                           "5. On the bottom, after the simulation is complete, you can see the actions the bot took.";

    QMessageBox::information(this, "Help - Instructions", instructions);
}
