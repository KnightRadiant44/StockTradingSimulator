#ifndef MEANREVERSION_H
#define MEANREVERSION_H
#include "MarketNeutralStrategy.h"
//Class relationship
class MeanReversionStrategy : public MarketNeutralStrategy {
private:
    double meanPrice = 100;
public:
    //Constructor initalising strategy name
    MeanReversionStrategy() : MarketNeutralStrategy("Mean Reversion") {}
    double execute(double currentPrice, const std::string& marketEvent) override {
        double action = (meanPrice - currentPrice) * 0.01;
        //Adjust mean price and action based on market events
        if (marketEvent.find("market downturn") != std::string::npos) {
            //Adjust mean price expectation during downturn
            meanPrice *= 0.95;
        } else if (marketEvent.find("bull market") != std::string::npos) {
            //Adjust mean price expectation during bull market
            meanPrice *= 1.05; 
        }
        return action;
    }
};
#endif //MEANREVERSION_H
