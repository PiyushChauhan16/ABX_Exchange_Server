# Build 
start server
1. node main.js

start client
1. cd ./abx_exchange_server/abx_exchange_server/client
2. g++ main.x.cpp -o main
3. ./main

NOTE: There will be a file called "output.log" which contains the final json and other relevant logs which are same as the output in the console. The default behaviour is to have file and console based logging enabled.

# Compiler
```
> g++ --version                                                                                                                                                     g++ (Ubuntu 11.4.0-1ubuntu1~22.04) 11.4.0            
```

# CPP Version
```
C++ 17
```

# Motivation
ABX_EXCHANGE_SERVER simulates a real-world HFT client-server architecture wherein client is responsible for building a json file containing an array of symbols received from the server (exchange).

# Features
1. templatized, configurable and maintainable.
2. human redable error handling along with file, line number and function which logged the error (Makes debugging super easy !).
3. file and console based looging support.

# Glimpse of log file
```
[ ABXMarketConnection.h : 53 ] INF newSymbol(): received new symbol packet
[ ABXMarketConnection.h : 41 ] INF dumpPacket(): [EXCHANGE PACKET]
[ ABXMarketConnection.h : 42 ] INF dumpPacket(): symbol:MSFT
[ ABXMarketConnection.h : 43 ] INF dumpPacket(): side:B
[ ABXMarketConnection.h : 44 ] INF dumpPacket(): quantity:20
[ ABXMarketConnection.h : 45 ] INF dumpPacket(): price:148
[ ABXMarketConnection.h : 46 ] INF dumpPacket(): seq_num:10
[ ABXMarketConnection.h : 47 ] INF dumpPacket(): 
[ SpinDispatcher.h : 89 ] INF process(): MarketConnection disconnected
[ TCPSocket.h : 93 ] DEB connect(): 4
[ TCPSocket.h : 103 ] INF connect(): connected to server
[ ABXMarketConnection.h : 135 ] INF operator()(): packet with seqnum '2' is missing
[ ABXMarketConnection.h : 135 ] INF operator()(): packet with seqnum '3' is missing
[ ABXMarketConnection.h : 135 ] INF operator()(): packet with seqnum '4' is missing
[ ABXMarketConnection.h : 135 ] INF operator()(): packet with seqnum '5' is missing
[ ABXMarketConnection.h : 135 ] INF operator()(): packet with seqnum '6' is missing
[ ABXMarketConnection.h : 135 ] INF operator()(): packet with seqnum '7' is missing
[ ABXMarketConnection.h : 135 ] INF operator()(): packet with seqnum '9' is missing
[ ABXMarketConnection.h : 145 ] INF operator()(): requesting packet with seqnum:11
[ TCPSocket.h : 128 ] DEB write(): 2 bytes written on wire 
```
# How does output looks like
```
 [
    {
        "symbol" : "AAPL",
        "side" : "B",
        "quantity" : 50,
        "price" : 100,
        "seq_num" : 1,
    },
    {
        "symbol" : "AAPL",
        "side" : "B",
        "quantity" : 30,
        "price" : 98,
        "seq_num" : 2,
    },
    {
        "symbol" : "AAPL",
        "side" : "S",
        "quantity" : 20,
        "price" : 101,
        "seq_num" : 3,
    },
    {
        "symbol" : "AAPL",
        "side" : "S",
        "quantity" : 10,
        "price" : 102,
        "seq_num" : 4,
    },
    {
        "symbol" : "META",
        "side" : "B",
        "quantity" : 40,
        "price" : 50,
        "seq_num" : 5,
    },
    {
        "symbol" : "META",
        "side" : "S",
        "quantity" : 30,
        "price" : 55,
        "seq_num" : 6,
    },
    {
        "symbol" : "META",
        "side" : "S",
        "quantity" : 20,
        "price" : 57,
        "seq_num" : 7,
    },
    {
        "symbol" : "MSFT",
        "side" : "B",
        "quantity" : 25,
        "price" : 150,
        "seq_num" : 8,
    },
    {
        "symbol" : "MSFT",
        "side" : "S",
        "quantity" : 15,
        "price" : 155,
        "seq_num" : 9,
    },
    {
        "symbol" : "MSFT",
        "side" : "B",
        "quantity" : 20,
        "price" : 148,
        "seq_num" : 10,
    },
]
```
