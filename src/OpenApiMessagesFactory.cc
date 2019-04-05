
#include "OpenApiMessages.pb.h"
#include "OpenApiModelMessages.pb.h"
#include "OpenApiCommonMessages.pb.h"
#include "OpenApiCommonModelMessages.pb.h"
#include "OpenApiMessagesFactory.h"

using namespace std;

ProtoMessage OpenApiMessagesFactory::GetMessage(string msg)
{
    ProtoMessage _msg;
    _msg.ParseFromString(msg);
    lastMessagePayloadType = _msg.payloadtype();
    lastMessagePayload = _msg.payload();
    return _msg;
}

ProtoMessage OpenApiMessagesFactory::CreateMessage(uint payloadType,
    string &payload, string &clientMsgId)
{
    ProtoMessage protoMsg;

    protoMsg.set_payloadtype(payloadType);
    protoMsg.set_payload(payload);
    protoMsg.set_clientmsgid(clientMsgId);

    return protoMsg;
}

ProtoMessage OpenApiMessagesFactory::CreateMessage(uint payloadType,
                string &payload)
{
    ProtoMessage protoMsg;

    protoMsg.set_payloadtype(payloadType);
    protoMsg.set_payload(payload);

    return protoMsg;
}

ProtoMessage OpenApiMessagesFactory::CreateAppAuthorizationRequest(string clientId,
    string clientSecret)
{
    ProtoOAApplicationAuthReq _msg;
    string msg_str;
    _msg.set_clientid(clientId);
    _msg.set_clientsecret(clientSecret);
    _msg.SerializeToString(&msg_str);
    return CreateMessage(_msg.payloadtype(), msg_str);
}
