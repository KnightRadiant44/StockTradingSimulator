#ifndef RANDOMSTRATEGY_H
#define RANDOMSTRATEGY_H
#include "MarketNeutralStrategy.h"
//Class relationship
class RandomStrategy : public MarketNeutralStrategy {
public:
    //Constructor
    RandomStrategy() : MarketNeutralStrategy("Random") {}
    //Execute moving average strategy
    double execute(double currentPrice, const std::string& marketEvent) override {
        //Determines random action
        double action = (rand() % 3 - 1) * currentPrice * 0.01;
        return action;
    }
};
#endif //RANDOMSTRATEGY_H
