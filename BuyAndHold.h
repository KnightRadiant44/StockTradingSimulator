#ifndef BUYANDHOLD_H
#define BUYANDHOLD_H
#include "DirectionalStrategy.h"
//Class relationship
class BuyAndHoldStrategy : public DirectionalStrategy {
public:
    //Constructor initalising strategy name
    BuyAndHoldStrategy() : DirectionalStrategy("Buy and Hold") {}
    //Executes buy and hold strategy
    double execute(double currentPrice, const std::string& marketEvent) override {
        double action = currentPrice * 0.01;
        //Adjust action based on market events
        if (marketEvent.find("bull market") != std::string::npos) {
            //Increase buying during bull market
            action *= 1.2;
        } else if (marketEvent.find("market downturn") != std::string::npos) {
             //Decrease buying during market downturn
            action *= 0.8;
        }
        return action;
    }
};
#endif //BUYANDHOLD_H
