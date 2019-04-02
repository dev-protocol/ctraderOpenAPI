
#include "OpenApiMessages.pb.h"
#include "OpenApiModelMessages.pb.h"
#include "OpenApiCommonMessages.pb.h"
#include "OpenApiCommonModelMessages.pb.h"

using namespace std;

class OpenApiMessagesFactory
{
    uint lastMessagePayloadType = 0;
    string lastMessagePayload = nullptr;

public:
    ProtoMessage CreateMessage(uint payloadType, ByteString payload = nullptr,
                string clientMsgId = nullptr)
    {
        //auto protoMsg = ProtoMessage.CreateBuilder();
        ProtoMessage protoMsg;
        protoMsg.set_payloadtype(payloadType);

        if (payload != nullptr)
            protoMsg.set_payload(payload);
        if (clientMsgId != nullptr)
            protoMsg.set_clientmsgid(clientMsgId);

        return protoMsg;
    }

    ProtoMessage CreateAppAuthorizationRequest(string clientId,
        string clientSecret, string clientMsgId = nullptr)
    {
        ProtoOAApplicationAuthReq _msg;
        _msg.set_clientid(clientId);
        _msg.set_clientsecret(clientSecret);
        return CreateMessage(_msg.payloadtype_,
                _msg.ToByteString(), clientMsgId);
    }
}
