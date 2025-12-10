import yfinance as yf
import pandas as pd
import numpy as np
import os

# -----------------------------------------------------------
# 1. SETTINGS
# -----------------------------------------------------------

TICKERS = ["AAPL", "MSFT", "GOOG"]

OPTION_EXAMPLES = [
    {"ticker": "AAPL", "quantity": 100, "strike": 180, "maturity": 0.5, "type": "CALL"},
    {"ticker": "MSFT", "quantity": 50,  "strike": 320, "maturity": 1.0, "type": "PUT"},
]

# <-- FIXED HISTORICAL WINDOW -->
START_DATE = "2023-01-01"
END_DATE   = "2023-07-01"

OUTPUT_DIR = "../"   # adjust if needed


# -----------------------------------------------------------
# 2. DOWNLOAD FIXED-DATE PRICE DATA
# -----------------------------------------------------------

print(f"Downloading market data from {START_DATE} to {END_DATE}...")

# Disable auto_adjust so we have clean OHLC
raw = yf.download(
    TICKERS,
    start=START_DATE,
    end=END_DATE,
    auto_adjust=False
)

# Use Close prices only
data = raw["Close"].dropna()

if data.empty:
    raise ValueError("Downloaded price data is empty! Check dates or ticker symbols.")

# -----------------------------------------------------------
# 3. CALCULATE SPOTS, VOLATILITIES, CORRELATIONS
# -----------------------------------------------------------

spots = data.iloc[-1]  # last price in the selected window

returns = data.pct_change().dropna()

vols = returns.std() * np.sqrt(252)

corr_matrix = returns.corr()


# -----------------------------------------------------------
# 4. SAVE OUTPUT FILES
# -----------------------------------------------------------

os.makedirs(OUTPUT_DIR, exist_ok=True)

# 4.1 market.csv
risk_free_rate = 0.03

mkt_df = pd.DataFrame({
    "ticker": TICKERS,
    "spot": [spots[t] for t in TICKERS],
    "vol":  [vols[t] for t in TICKERS],
    "r": risk_free_rate
})

mkt_df.to_csv(OUTPUT_DIR + "market.csv", index=False)
print("Saved:", OUTPUT_DIR + "market.csv")


# 4.2 correlation.csv
corr_matrix.to_csv(OUTPUT_DIR + "correlation.csv")
print("Saved:", OUTPUT_DIR + "correlation.csv")


# 4.3 portfolio.csv
portfolio = []

# Stocks — 100 shares each
for t in TICKERS:
    portfolio.append({
        "type": "STOCK",
        "ticker": t,
        "quantity": 100,
        "strike": "",
        "maturity": "",
        "option_type": ""
    })

# Options
for opt in OPTION_EXAMPLES:
    portfolio.append({
        "type": "OPTION",
        "ticker": opt["ticker"],
        "quantity": opt["quantity"],
        "strike": opt["strike"],
        "maturity": opt["maturity"],
        "option_type": opt["type"]
    })

portfolio_df = pd.DataFrame(portfolio)
portfolio_df.to_csv(OUTPUT_DIR + "portfolio.csv", index=False)
print("Saved:", OUTPUT_DIR + "portfolio.csv")


print("\nAll fixed-date data generated successfully!")
