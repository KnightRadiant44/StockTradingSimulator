#ifndef MEANREVERSION_H
#define MEANREVERSION_H
#include "MarketNeutralStrategy.h"
class MeanReversionStrategy : public MarketNeutralStrategy {
private:
    double meanPrice = 100;
public:
    MeanReversionStrategy() : MarketNeutralStrategy("Mean Reversion") {}
    double execute(double currentPrice, const std::string& marketEvent) override {
        double action = (meanPrice - currentPrice) * 0.01;
        // Adjust mean price and action based on market events
        if (marketEvent.find("market downturn") != std::string::npos) {
            meanPrice *= 0.95; // Adjust mean price expectation during downturn
        } else if (marketEvent.find("bull market") != std::string::npos) {
            meanPrice *= 1.05; // Adjust mean price expectation during bull market
        }
        return action;
    }
};
#endif // MEANREVERSION_H
