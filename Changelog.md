# Changelog

## Reason: Class Relationships

- Combined the three `RiskManagement` classes into one class, as the previous inheritance was unnecessary.

- Introduced three levels of inheritance in the trading strategies:
  - **Trading Strategy (Base Class)**
    - **MarketNeutralStrategy (Inherits from TradingStrategy)**
      - **MeanReversionStrategy (Inherits from MarketNeutralStrategy)**
      - **RandomStrategy (Inherits from MarketNeutralStrategy)**
    - **DirectionalStrategy (Inherits from TradingStrategy)**
      - **Buy&HoldStrategy (Inherits from DirectionalStrategy)**
      - **MovingAverageStrategy (Inherits from DirectionalStrategy)**
      - **TrendFollowingStrategy (Inherits from DirectionalStrategy)**
