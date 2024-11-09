#ifndef MOVINGAVG_H
#define MOVINGAVG_H
#include "DirectionalStrategy.h"
//Class relationship
class MovingAverageStrategy : public DirectionalStrategy {
private:
    double movingAvg = 100;
public:
    //Constructor
    MovingAverageStrategy() : DirectionalStrategy("Moving Average") {}
    //Executes moving average strategy
    double execute(double currentPrice, const std::string& marketEvent) override {
        //Adjusts moving average based on market factors
        movingAvg = 0.9 * movingAvg + 0.1 * currentPrice;
        //Calculates action
        double action = (currentPrice - movingAvg) * 0.01;
        return action;
    }
};
#endif // MOVINGAVG_H
