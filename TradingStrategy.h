#ifndef TRADINGSTRATEGY_H
#define TRADINGSTRATEGY_H
#include <string>
class TradingStrategy {
protected:
    std::string name;
public:
    //Constructor
    TradingStrategy(const std::string& n) : name(n) {}
    //Virtual function to execute strategy
    virtual double execute(double currentPrice, const std::string& marketEvent) = 0;
    //Name getter
    std::string getName() const { return name; }
    //Virtual destructor
    virtual ~TradingStrategy() = default;
};
#endif //TRADINGSTRATEGY_H
