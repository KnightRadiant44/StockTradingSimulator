#ifndef MARKET_H
#define MARKET_H
#include <string>
#include <random>
#include <ctime>
#include <algorithm>
#include <cstdlib>
#include <cmath>
#include <algorithm> // For std::max_element, std::min_element
class Market {
private:
    double price;
    double volatility;
    std::mt19937 gen;
    std::normal_distribution<> d;
    double overallTrend;
    double currentPrice;
public:

    double getPrice() const { return currentPrice; }  // Add 'const' here
    double getVolatility() const { return volatility; }  // Make sure this is const too
    Market() : price(100), volatility(0.02), gen(std::random_device{}()), d(0, 1), overallTrend(0) {}

    double getPrice() {
        double drift = 0.0001 + overallTrend;
        price *= std::exp((drift - 0.5 * volatility * volatility) + volatility * d(gen));
        return price;
    }

    void setVolatility(double vol) { volatility = vol; }

    void adjustVolatility(const std::string& marketEvent) {
        if (marketEvent.find("uncertainty") != std::string::npos) {
            volatility *= 1.2;
        } else if (marketEvent.find("bull market") != std::string::npos) {
            volatility *= 0.9;
        }
        volatility = std::min(std::max(volatility, 0.01), 0.05);
    }

    void adjustOverallTrend(const std::string& marketEvent) {
        if (marketEvent.find("bull market") != std::string::npos) {
            overallTrend += 0.0002;
        } else if (marketEvent.find("market downturn") != std::string::npos) {
            overallTrend -= 0.0002;
        }
        overallTrend = std::min(std::max(overallTrend, -0.001), 0.001);
    }
};

#endif // MARKET_H
