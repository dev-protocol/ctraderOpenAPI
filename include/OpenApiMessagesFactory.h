
#include <string>
#include "OpenApiMessages.pb.h"
#include "OpenApiModelMessages.pb.h"
#include "OpenApiCommonMessages.pb.h"
#include "OpenApiCommonModelMessages.pb.h"

using namespace std;

class OpenApiMessagesFactory
{
    uint lastMessagePayloadType;
    string lastMessagePayload;

public:

    string getLastMessage(void);
    ProtoMessage GetMessage(string msg);
    ProtoMessage CreateMessage(uint payloadType, string &payload,
                string &clientMsgId);
    ProtoMessage CreateMessage(uint payloadType, string &payload);

    ProtoMessage CreateAppAuthorizationRequest(string clientId,
        string clientSecret);
    ProtoMessage CreateAccAuthorizationRequest(string token,
        long accountId);
    ProtoMessage CreateAccountListRequest(string token);
    ProtoMessage CreateSubscribeForSpotsRequest(long accountId, int symbolId);
    ProtoMessage CreateUnsubscribeFromSpotsRequest(long accountId,int symbolId);
    ProtoMessage CreateTraderRequest(long accountID);
    ProtoMessage CreateHeartbeatEvent(void);
    ProtoMessage CreateMarketOrderRequest(long accountId, string accessToken,
    int symbolId, ProtoOATradeSide tradeSide, long volume);
    ProtoMessage CreateReconcileRequest(long accountId);
    ProtoMessage CreateTickDataRequest(long accountId, int symbolId, long from,
        long to, ProtoOAQuoteType type);

    ProtoOAExecutionEvent GetExecutionEvent(string msg);
};
