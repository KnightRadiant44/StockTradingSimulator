#ifndef RISKMANAGEMENT_H
#define RISKMANAGEMENT_H
#include <string>
class RiskManagement {
private:
    //Member variables listing different risk factors
    double maxLoss;
    double riskTolerance;
    double volatility;
public:
    //Constructor
    RiskManagement(double ml, double vol) : maxLoss(ml), riskTolerance(1.0), volatility(vol) {}
    //Adjust postion based on risk factors
    double adjustPosition(double suggestedPosition, double currentPrice, double balance) {
        //Calculaes the adjustedPosition based on a variety of factors
        double maxPosition = balance * maxLoss / currentPrice * riskTolerance;
        double adjustedPosition = std::min(std::abs(suggestedPosition), maxPosition) * (suggestedPosition >= 0 ? 1 : -1);
        adjustedPosition *= (1 - volatility);
        return adjustedPosition;
    }
    //Adjust risk tolerance based on market events
    void adjustRiskTolerance(const std::string& marketEvent) {
        if (marketEvent.find("uncertainty") != std::string::npos) {
            //Decrease risk tolerance during uncertainity
            riskTolerance *= 0.9;
        } else if (marketEvent.find("bull market") != std::string::npos) {
            //Increase risk tolerance during bull market
            riskTolerance *= 1.1;
        }
        //Keep risk tolerance between 0.5 and 1.5
        riskTolerance = std::min(std::max(riskTolerance, 0.5), 1.5);  // Keep risk tolerance between 0.5 and 1.5
    }
    //Set the volatility
    void setVolatility(double vol) {
        volatility = vol;
    }
    //Get the volatility
    double getVolatility() const {
        return volatility;
    }
};
#endif //RISKMANAGEMENT_H
