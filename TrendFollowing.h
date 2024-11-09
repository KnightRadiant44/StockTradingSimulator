#ifndef TRENDFOLLOWING_H
#define TRENDFOLLOWING_H
#include "DirectionalStrategy.h"
//Class relationship
class TrendFollowingStrategy : public DirectionalStrategy {
public:
    //Constructor
    TrendFollowingStrategy() : DirectionalStrategy("Trend Following") {}
    //Executes strategy
    double execute(double currentPrice, const std::string& marketEvent) override {
        double action = (currentPrice - 100) * 0.01;
        //Adjust action based on market events
        if (marketEvent.find("bull market") != std::string::npos) {
            //Increase trend following in bull market
            action *= 1.3;
        } else if (marketEvent.find("market downturn") != std::string::npos) {
            //Decrease trend following in market downturn
            action *= 0.7; 
        }
        return action;
    }
};
#endif //TRENDFOLLOWING_H
