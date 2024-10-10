    #ifndef TRADINGSTRATEGY_H
#define TRADINGSTRATEGY_H
#include <string>
class TradingStrategy {
protected:
    std::string name;
public:
    TradingStrategy(const std::string& n) : name(n) {}
    virtual double execute(double currentPrice, const std::string& marketEvent) = 0;
    std::string getName() const { return name; }
    virtual ~TradingStrategy() = default;
};
#endif // TRADINGSTRATEGY_H
