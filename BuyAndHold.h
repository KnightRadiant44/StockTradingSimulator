#ifndef BUYANDHOLD_H
#define BUYANDHOLD_H
#include "DirectionalStrategy.h"
class BuyAndHoldStrategy : public DirectionalStrategy {
public:
    BuyAndHoldStrategy() : DirectionalStrategy("Buy and Hold") {}
    double execute(double currentPrice, const std::string& marketEvent) override {
        double action = currentPrice * 0.01;
        // Adjust action based on market events
        if (marketEvent.find("bull market") != std::string::npos) {
            action *= 1.2; // Increase buying during bull market
        } else if (marketEvent.find("market downturn") != std::string::npos) {
            action *= 0.8; // Decrease buying during market downturn
        }
        return action;
    }
};
#endif // BUYANDHOLD_H
