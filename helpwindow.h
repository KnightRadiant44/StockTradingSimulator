#ifndef HELPWINDOW_H
#define HELPWINDOW_H
#include <QWidget>
#include <QStackedWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>

class HelpWindow : public QWidget {
    Q_OBJECT

public:
    HelpWindow(QWidget *parent = nullptr) : QWidget(parent) {
        QStackedWidget *stackedWidget = new QStackedWidget;

        // Page 1 - Instructions
        QWidget *page1 = new QWidget;
        QVBoxLayout *layout1 = new QVBoxLayout;
        QLabel *instructions = new QLabel("Instructions:\n\n"
                                          "1. Select a strategy to use on the top right.\n"
                                          "2. Choose the number of days to simulate.\n"
                                          "3. Enter how frequently updates occur.\n"
                                          "4. View stats and trading info on the left.\n"
                                          "5. After simulation, see bot actions at the bottom.");
        QPushButton *nextButton = new QPushButton("Next");
        layout1->addWidget(instructions);
        layout1->addWidget(nextButton);
        page1->setLayout(layout1);

        // Page 2 - Technical Terms
        QWidget *page2 = new QWidget;
        QVBoxLayout *layout2 = new QVBoxLayout;
        QLabel *technicalTerms = new QLabel("Technical Terms:\n\n"
                                            "Balance: Your current cash balance.\n"
                                            "Profit & Loss: Your net earnings/losses.\n"
                                            "Cumulative Return: Total return from start.\n"
                                            "Drawdown: Maximum drop from peak.\n"
                                            "Volatility: Market fluctuation level.\n"
                                            "Sharpe Ratio: Risk-adjusted return.\n"
                                            "Current Price: Price of selected asset.\n"
                                            "Market Event: Recent notable event.");
        QPushButton *backButton = new QPushButton("Back");
        layout2->addWidget(technicalTerms);
        layout2->addWidget(backButton);
        page2->setLayout(layout2);

        // Add pages to the stacked widget
        stackedWidget->addWidget(page1);
        stackedWidget->addWidget(page2);

        // Main layout
        QVBoxLayout *mainLayout = new QVBoxLayout;
        mainLayout->addWidget(stackedWidget);
        setLayout(mainLayout);

        // Connections
        connect(nextButton, &QPushButton::clicked, [=]() { stackedWidget->setCurrentIndex(1); });
        connect(backButton, &QPushButton::clicked, [=]() { stackedWidget->setCurrentIndex(0); });
    }
};

#endif // HELPWINDOW_H
