
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
};
