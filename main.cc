#include <iostream>
#include <fstream>
#include <string>
#include "OpenApiMessages.pb.h"
#include "OpenApiModelMessages.pb.h"
#include "teste1.h"

using namespace std;

string clientId;
string clientSecret;
string accessToken;
long ctid;

private ProtoOAApplicationAuthReq createAuthorizationRequest(string clientId, string clientSecret)
{
    return ProtoOAApplicationAuthReq.newBuilder()
            .setClientId(clientId)
            .setClientSecret(clientSecret)
            .build();
}

private ProtoOAAccountAuthReq createAccountAuthorizationRequest(string accessToken, long ctidTraderAccountId)
{
    return ProtoOAAccountAuthReq.newBuilder()
            .setAccessToken(accessToken)
            .setCtidTraderAccountId(ctidTraderAccountId)
            .build();
}

public MessageLite authorizeApplication(string clientId, string clientSecret) throws InterruptedException
{
    ProtoOAApplicationAuthReq appAuthReq = createAuthorizationRequest(clientId, clientSecret);
    ProtoMessageReceiver receiver = nettyClient.writeAndFlush(appAuthReq);
    return receiver.waitSingleResult();
}

public MessageLite authorizeAccount(long ctidTraderAccountId, string accessToken) throws InterruptedException
{
    ProtoOAAccountAuthReq accountAuthorizationRequest = createAccountAuthorizationRequest(accessToken, ctidTraderAccountId);
    ProtoMessageReceiver receiver = nettyClient.writeAndFlush(accountAuthorizationRequest);
    return receiver.waitSingleResult();
}
    
void sendAuthorizationRequest(NettyClient nettyClient) throws InterruptedException 
{
    try {

        //AuthHelper authHelper = nettyClient.getAuthHelper();

        MessageLite applicationAuthRes = authorizeApplication(clientId, clientSecret);
        if (applicationAuthRes instanceof ProtoOAApplicationAuthRes) {
            cout << "Response ProtoOAApplicationAuthRes received." << endl;
            cout << "Response: " + ((ProtoOAApplicationAuthRes) applicationAuthRes).getPayloadType() << endl;
        } else {
            cout << "Something went wrong" << endl;
            cout << "Response: " + applicationAuthRes << endl;
            throw new AuthorizationException("application can't be authorize");
        }

        MessageLite accountAuthRes = authorizeAccount(ctid, accessToken);
        if (accountAuthRes instanceof ProtoOAAccountAuthRes) {
            std:cout << ("Response ProtoOAAccountAuthRes received.") << endl;
            std:cout << ("Response: " + accountAuthRes) << endl;
        } else {
            std:cout << ("Something went wrong.") << endl;
            std:cout << ("Response: " + accountAuthRes) << endl;
        }
    } finally {
        nettyClient.closeConnection();
    }
}

int main(int argc, char* argv[])
{
    clientId = "777";
    clientSecret = "fyJizNAcVXiGA-gFqmTnjkYDgqKXh32p35S5u_7CsnM";
    accessToken = "cYpgWvzRxrhAFkNIT6plB7zV5uc-jRxTDpre_074mSw";
    ctid = ;
    return 0;
}
