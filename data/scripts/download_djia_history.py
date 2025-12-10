import yfinance as yf
import os
import time
import pandas as pd

tickers = [
    "GS","CAT","MSFT","AXP","HD","SHW","V","UNH","AMGN","JPM",
    "MCD","IBM","TRV","AAPL","CRM","AMZN","JNJ","BA","HON","NVDA",
    "MMM","CVX","PG","WMT","DIS","MRK","CSCO","KO","NKE","VZ"
]

output_dir = "../history/djia"
os.makedirs(output_dir, exist_ok=True)

for i, ticker in enumerate(tickers, 1):
    print(f"[{i}/{len(tickers)}] Downloading {ticker}...")

    df = yf.download(
        ticker,
        start="1980-01-01",
        interval="1d",
        progress=False,
        auto_adjust=False,
        group_by="ticker"    # важно!
    )

    if df.empty:
        print(f"WARNING: No data for {ticker}")
        continue

    # 💡 УДАЛЯЕМ мультииндекс, если он вдруг создался
    if isinstance(df.columns, pd.MultiIndex):
        df.columns = df.columns.droplevel(0)

    # 💡 Выстраиваем ЧЁТКИЙ порядок колонок
    df = df[["Open", "High", "Low", "Close", "Adj Close", "Volume"]]

    # 💡 УБЕЖДАЕМСЯ, что индекс — это Date
    df.index.name = "Date"

    # 💡 Сохраняем вручную — так мы исключаем все кривые варианты
    df.to_csv(f"{output_dir}/{ticker}.csv")

    print(f"Saved {ticker}.csv with {len(df)} rows")

    time.sleep(1)
