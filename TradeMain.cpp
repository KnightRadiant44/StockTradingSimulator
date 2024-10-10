#include <iostream>
#include <ctime>
#include <cstdlib>
#include "TradingBot.h"
int main() {
    std::srand(std::time(nullptr));
    TradingBot bot;

    while (true) {
        std::cout << "\n1. Select strategy" << std::endl;
        std::cout << "2. Run simulation" << std::endl;
        std::cout << "3. Exit" << std::endl;
        std::cout << "Enter your choice: ";

        int choice;
        std::cin >> choice;

        if (choice == 1) {
            bot.displayStrategies();
            std::cout << "Select strategy (1-5): ";
            int strategyChoice;
            std::cin >> strategyChoice;
            bot.setStrategy(strategyChoice - 1);
        } else if (choice == 2) {
            std::cout << "Enter number of days to simulate: ";
            int days;
            std::cin >> days;
            bot.executeStrategy(days);
        } else if (choice == 3) {
            break;
        } else {
            std::cout << "Invalid choice. Please try again." << std::endl;
        }
    }

    return 0;
}
