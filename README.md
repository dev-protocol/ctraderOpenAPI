# ctraderOpenAPI
Ctrader Open API C++ client

Ctrader Open API 2.0 is an API created for trading API in any broker supporting Ctrader.
Visit https://www.spotware.com/ctrader/trading/ctrader-open-api-trading

This is a C++ Open API 2.0 client template for those dont want to use C# or Java examples.
It was compiled in Debian 9 with g++ compiler.

Copy the headers to protobuf folder after install the dependencies:
cp include/*.h /usr/include/google/protobuf

Also define your credentials in src/credentials.h

# Dependencies
apt-get install libssl-dev libprotoc-dev

# Optional dependencies
apt-get install protobuf-compiler libprotobuf-c-dev
