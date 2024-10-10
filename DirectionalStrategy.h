#ifndef DIRECTIONALSTRATEGY_H
#define DIRECTIONALSTRATEGY_H
#include "TradingStrategy.h"
class DirectionalStrategy : public TradingStrategy {
public:
    DirectionalStrategy(const std::string& n) : TradingStrategy(n) {}
    virtual double execute(double currentPrice, const std::string& marketEvent) override = 0;
    virtual ~DirectionalStrategy() = default;
};
#endif // DIRECTIONALSTRATEGY_H
