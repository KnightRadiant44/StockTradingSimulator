#ifndef MARKETNEUTRALSTRATEGY_H
#define MARKETNEUTRALSTRATEGY_H
#include "TradingStrategy.h"
//Class relationship
class MarketNeutralStrategy : public TradingStrategy {
public:
    //Constructor initialising strategy name
    MarketNeutralStrategy(const std::string& n) : TradingStrategy(n) {}
    //Virtual function to execute the strategy
    virtual double execute(double currentPrice, const std::string& marketEvent) override = 0;
    //Virtual destructor
    virtual ~MarketNeutralStrategy() = default;
};
#endif //MARKETNEUTRALSTRATEGY_H
