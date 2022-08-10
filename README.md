# ctraderOpenAPI
Ctrader Open API C++ client template

Ctrader Open API 2.0 is an API created by Spotware for algorithmic trading and it can be
used in any broker supporting Ctrader. Visit https://www.spotware.com/ctrader/trading/ctrader-open-api-trading. 

This is a C++ Open API 2.0 client template for those dont want to use C# or Java examples.
It was compiled in Debian 9 with g++ compiler. It should work fine in any Linux system.

Define your credentials in src/credentials.h

# Build protobuf compiler
This example will not compile if you dont have protobuf compiler and the headers in your system. 
So follow the instructions here: https://github.com/protocolbuffers/protobuf/blob/master/src/README.md

The Makefile inside src folder set libraries and headers to /usr/local folder. If you have installed protobuf
in another folder, change src/Makefile:

Define protobuf header folder:
OPT += -I /usr/local/include/google/protobuf

Define protobuf libraries folder:
LIBS += -L /usr/local/lib

# Generating C++ files from protobuf
Download the last files here: https://github.com/spotware/openapi-proto-messages
Run these commands inside the project folder and --proto_path indicate the folder where is .proto files
protoc --cpp_out=./src/ --proto_path=./protobuf OpenApiMessages.proto

protoc --cpp_out=./src/ --proto_path=./protobuf OpenApiModelMessages.proto

protoc --cpp_out=./src/ --proto_path=./protobuf OpenApiCommonMessages.proto

protoc --cpp_out=./src/ --proto_path=./protobuf OpenApiCommonModelMessages.prot

mv src/*.h include/

# Donations

1. Bitcoin address: 3LVEhsDKBQwGE6WfCWGXvvK5m99CFsQR9v

2. Ethereum address: 0x80aac844f39b478c699d1c0c8bc2e6c9c9e02026

3. Tron address: TKLVUCkcfrqCyqRP3vR5UCNkMqCwWZwmmU

4. XRP address: rnW8je5SsuFjkMSWkgfXvqZH3gLTpXxfFH

   XRP tag: 100558856
