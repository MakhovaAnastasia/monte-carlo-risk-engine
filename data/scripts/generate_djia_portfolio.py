import pandas as pd
import os
import datetime

# === CONFIG ===
HISTORY_DIR = "../history/djia"
SNAPSHOT_DATE = "2025-08-08"
PORTFOLIO_VALUE = 10_000_000    # 10 million USD
OUTPUT_CSV = "../portfolio_djia.csv"

# Dow Jones tickers
TICKERS = [
    "GS","CAT","MSFT","AXP","HD","SHW","V","UNH","AMGN","JPM",
    "MCD","IBM","TRV","AAPL","CRM","AMZN","JNJ","BA","HON","NVDA",
    "MMM","CVX","PG","WMT","DIS","MRK","CSCO","KO","NKE","VZ"
]

def load_price(ticker, snapshot_date):
    """Load single closing price at target date."""
    path = os.path.join(HISTORY_DIR, f"{ticker}.csv")
    df = pd.read_csv(path, parse_dates=["Date"])
    row = df.loc[df["Date"] == snapshot_date]

    if row.empty:
        raise ValueError(f"No data for {ticker} on {snapshot_date}")

    return float(row["Close"].iloc[0])


def main():
    print(f"Generating DJIA portfolio for {SNAPSHOT_DATE}...")
    prices = {}

    # 1. Load prices on snapshot date
    for ticker in TICKERS:
        try:
            p = load_price(ticker, SNAPSHOT_DATE)
            prices[ticker] = p
            print(f"{ticker}: {p}")
        except Exception as e:
            print(f"ERROR {ticker}: {e}")
            return

    # 2. Compute DJIA weights = price / sum(price)
    total_price_sum = sum(prices.values())
    weights = {t: prices[t] / total_price_sum for t in TICKERS}

    # 3. Compute quantities
    quantities = {
        t: int((weights[t] * PORTFOLIO_VALUE) / prices[t])
        for t in TICKERS
    }

    # 4. Create portfolio DataFrame
    df = pd.DataFrame({
        "type": ["STOCK"] * len(TICKERS),
        "ticker": TICKERS,
        "quantity": [quantities[t] for t in TICKERS],
        "strike": ["" for _ in TICKERS],
        "maturity": ["" for _ in TICKERS],
        "option_type": ["" for _ in TICKERS]
    })

    df.to_csv(OUTPUT_CSV, index=False)
    print(f"\nPortfolio saved to {OUTPUT_CSV}\n")

    # Summary
    print("=== Portfolio Summary ===")
    for t in TICKERS:
        print(f"{t}: price={prices[t]}, weight={weights[t]:.4f}, qty={quantities[t]}")

    print("\nDONE.")


if __name__ == "__main__":
    main()
