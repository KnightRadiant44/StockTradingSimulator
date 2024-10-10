#ifndef RANDOMSTRATEGY_H
#define RANDOMSTRATEGY_H
#include "MarketNeutralStrategy.h"
class RandomStrategy : public MarketNeutralStrategy {
public:
    RandomStrategy() : MarketNeutralStrategy("Random") {}
    double execute(double currentPrice, const std::string& marketEvent) override {
        double action = (rand() % 3 - 1) * currentPrice * 0.01;
        return action;
    }
};
#endif // RANDOMSTRATEGY_H
