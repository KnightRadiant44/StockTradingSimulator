#ifndef MOVINGAVG_H
#define MOVINGAVG_H
#include "DirectionalStrategy.h"
class MovingAverageStrategy : public DirectionalStrategy {
private:
    double movingAvg = 100;
public:
    MovingAverageStrategy() : DirectionalStrategy("Moving Average") {}
    double execute(double currentPrice, const std::string& marketEvent) override {
        movingAvg = 0.9 * movingAvg + 0.1 * currentPrice;
        double action = (currentPrice - movingAvg) * 0.01;
        return action;
    }
};

#endif // MOVINGAVG_H
