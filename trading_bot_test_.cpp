#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include "TradingBot.h"

// Simple test framework
class TestSuite {
private:
    std::vector<std::pair<std::string, std::function<bool()>>> tests;
    int passedTests = 0;
    int totalTests = 0;

public:
    void addTest(const std::string& name, std::function<bool()> test) {
        tests.push_back({name, test});
    }

    void runTests() {
        for (const auto& test : tests) {
            std::cout << "Running test: " << test.first << "... ";
            if (test.second()) {
                std::cout << "PASSED" << std::endl;
                passedTests++;
                std::cout << "Test " << test.first << " passed!" << std::endl; // Added message for passing tests
            } else {
                std::cout << "FAILED" << std::endl;
            }
            totalTests++;
        }
        std::cout << "\nTest results: " << passedTests << " passed out of " << totalTests << " total tests." << std::endl;
    }
};

// Helper macros for assertions
#define ASSERT_EQ(a, b) if ((a) != (b)) { std::cout << "Assertion failed: " << #a << " != " << #b << std::endl; return false; }
#define ASSERT_DOUBLE_EQ(a, b) if (std::abs((a) - (b)) > 1e-6) { std::cout << "Assertion failed: " << #a << " != " << #b << std::endl; return false; }

// Unit tests for TradingBot
int test() {
    TestSuite suite;

    // Test 1: Initial state of the TradingBot
    suite.addTest("InitialState", []() {
        TradingBot bot("TestUser");
        ASSERT_DOUBLE_EQ(bot.getBalance(), 10000.0);
        ASSERT_EQ(bot.getOwnedStocks(), 0);
        ASSERT_DOUBLE_EQ(bot.getTotalReturn(), 0.0);
        ASSERT_DOUBLE_EQ(bot.getMaxDrawdown(), 0.0);
        ASSERT_DOUBLE_EQ(bot.getVolatility(), 0.0);
        ASSERT_DOUBLE_EQ(bot.getSharpeRatio(), 0.0);
        return true;
    });

    // Test 2: Setting and getting strategy
    suite.addTest("SetAndGetStrategy", []() {
        TradingBot bot("TestUser");
        bot.setStrategy(0);  // Assuming 0 is Buy and Hold
        ASSERT_EQ(bot.getCurrentStrategyName(), "Buy and Hold");
        bot.setStrategy(1);  // Assuming 1 is Mean Reversion
        ASSERT_EQ(bot.getCurrentStrategyName(), "Mean Reversion");
        return true;
    });

    // Test 3: Simulation initialization
    suite.addTest("SimulationInitialization", []() {
        TradingBot bot("TestUser");
        bot.initializeSimulation(100);
        ASSERT_EQ(bot.getCurrentDay(), 0);
        ASSERT_EQ(bot.isSimulationComplete(), false);
        return true;
    });

    // Test 4: Executing a trading day
    suite.addTest("ExecuteTradingDay", []() {
        TradingBot bot("TestUser");
        bot.initializeSimulation(100);
        bot.setStrategy(0);  // Set to Buy and Hold for predictable behavior
        double initialBalance = bot.getBalance();
        bot.executeNextDay();
        ASSERT_EQ(bot.getCurrentDay(), 1);
        ASSERT_EQ(bot.getBalance() != initialBalance, true);  // Balance should change after a day of trading
        return true;
    });

    // Test 5: Simulation completion
    suite.addTest("SimulationCompletion", []() {
        TradingBot bot("TestUser");
        bot.initializeSimulation(5);
        for (int i = 0; i < 5; ++i) {
            bot.executeNextDay();
        }
        ASSERT_EQ(bot.isSimulationComplete(), true);
        ASSERT_EQ(bot.getCurrentDay(), 5);
        return true;
    });

    // Run all tests
    suite.runTests();

    return 0;
}


// Main function to run the tests
int main1() {
    return test() ? 0 : 1; // Return 0 if tests passed, 1 otherwise
}
