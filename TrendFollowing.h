#ifndef TRENDFOLLOWING_H
#define TRENDFOLLOWING_H
#include "DirectionalStrategy.h"
class TrendFollowingStrategy : public DirectionalStrategy {
public:
    TrendFollowingStrategy() : DirectionalStrategy("Trend Following") {}
    double execute(double currentPrice, const std::string& marketEvent) override {
        double action = (currentPrice - 100) * 0.01;
        // Adjust action based on market events
        if (marketEvent.find("bull market") != std::string::npos) {
            action *= 1.3; // Increase trend following in bull market
        } else if (marketEvent.find("market downturn") != std::string::npos) {
            action *= 0.7; // Decrease trend following in market downturn
        }
        return action;
    }
};
#endif // TRENDFOLLOWING_H
