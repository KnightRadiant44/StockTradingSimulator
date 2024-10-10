#ifndef RISKMANAGEMENT_H
#define RISKMANAGEMENT_H
#include <string>
class RiskManagement {
private:
    double maxLoss;
    double riskTolerance;
    double volatility;
public:
    RiskManagement(double ml, double vol) : maxLoss(ml), riskTolerance(1.0), volatility(vol) {}

    double adjustPosition(double suggestedPosition, double currentPrice, double balance) {
        double maxPosition = balance * maxLoss / currentPrice * riskTolerance;
        double adjustedPosition = std::min(std::abs(suggestedPosition), maxPosition) * (suggestedPosition >= 0 ? 1 : -1);
        adjustedPosition *= (1 - volatility);
        return adjustedPosition;
    }

    // Adjust risk tolerance based on market events
    void adjustRiskTolerance(const std::string& marketEvent) {
        if (marketEvent.find("uncertainty") != std::string::npos) {
            riskTolerance *= 0.9;
        } else if (marketEvent.find("bull market") != std::string::npos) {
            riskTolerance *= 1.1;
        }
        riskTolerance = std::min(std::max(riskTolerance, 0.5), 1.5);  // Keep risk tolerance between 0.5 and 1.5
    }

    void setVolatility(double vol) {
        volatility = vol;
    }

    double getVolatility() const {
        return volatility;
    }
};
#endif // RISKMANAGEMENT_H
