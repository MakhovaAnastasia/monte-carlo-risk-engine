FROM ubuntu:22.04

# --- отключаем интерактивные вопросы ---
ARG DEBIAN_FRONTEND=noninteractive
ENV TZ=UTC

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    doxygen \
    graphviz \
    git
    #pandoc
    #texlive \
    #texlive-latex-base \
    #texlive-latex-extra \
    #texlive-fonts-recommended \
    #texlive-fonts-extra

WORKDIR /app
COPY . .

# 1) Build project
RUN mkdir -p build && cd build && cmake .. && make -j4

# 2) Generate docs AFTER build (risk_engine will NOT run here)
#RUN doxygen Doxyfile
#
#RUN make -C docs/latex pdf || true

# 3) Copy PDF to root
#RUN cp docs/latex/refman.pdf manual.pdf || true

#RUN pandoc -f docbook -t markdown \
#    -o /app/manual.md \
#    /app/docs/docbook/index.xml || true


# 4) Only now define ENTRYPOINT — does NOT run during build
ENTRYPOINT ["./build/risk_engine"]

# Default args (override with docker run ...)
CMD ["--djia", "2025-08-08", "--history", "data/history/djia", "--value", "10000000", "--lookback", "252", "--horizon", "10", "--scenarios", "10000", "--confidence", "0.95", "--out", "data/portfolio_djia.csv"]