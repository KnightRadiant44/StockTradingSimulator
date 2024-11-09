#ifndef DIRECTIONALSTRATEGY_H
#define DIRECTIONALSTRATEGY_H
#include "TradingStrategy.h"
//Class relationship
class DirectionalStrategy : public TradingStrategy {
public:
    //Constructor initialising strategy name
    DirectionalStrategy(const std::string& n) : TradingStrategy(n) {}
    //Virtual function to execute the strategy
    virtual double execute(double currentPrice, const std::string& marketEvent) override = 0;
    //Virtual destructor
    virtual ~DirectionalStrategy() = default;
};
#endif //DIRECTIONALSTRATEGY_H
