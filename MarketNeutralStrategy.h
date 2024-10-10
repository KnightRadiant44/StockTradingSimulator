#ifndef MARKETNEUTRALSTRATEGY_H
#define MARKETNEUTRALSTRATEGY_H
#include "TradingStrategy.h"
class MarketNeutralStrategy : public TradingStrategy {
public:
    MarketNeutralStrategy(const std::string& n) : TradingStrategy(n) {}
    virtual double execute(double currentPrice, const std::string& marketEvent) override = 0;
    virtual ~MarketNeutralStrategy() = default;
};
#endif // MARKETNEUTRALSTRATEGY_H
