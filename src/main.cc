#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <iterator>
#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>

#define _BSD_SOURCE
#include <netdb.h>

#include "OpenApiMessages.pb.h"
#include "OpenApiModelMessages.pb.h"
#include "OpenApiCommonMessages.pb.h"
#include "OpenApiCommonModelMessages.pb.h"
#include "OpenApiMessagesFactory.h"
#include "credentials.h"

#define CHK_NULL(x) if ((x)==NULL) exit (1)
#define CHK_ERR(err,s) if ((err)==-1) { perror(s); exit(1); }
#define CHK_SSL(err) if ((err)==-1) { ERR_print_errors_fp(stderr); exit(2); }

using namespace google::protobuf::io;
using namespace std;

// ctrader vars
string clientId;
string clientSecret;
string accessToken;
long _accountID;
string _apiHost = "demo.ctraderapi.com";
int _apiPort = 5035;
int MaxMessageSize = 1000000;
long testOrderId = -1;
long testPositionId = -1;
list <ProtoOACtidTraderAccount> _accounts;
list <ProtoOATrader> _traders;

pthread_t thread_sslread;
pthread_attr_t tattr;
pthread_mutex_t mutex_resp;
pthread_cond_t wait_resp;

// SSL vars
int sd;
SSL_CTX* ctx;
SSL*     ssl;
char buf[4096];


void printfCertInfo(SSL *sslx)
{
    char*    str;
    X509*    server_cert;
    //EVP_PKEY *pubkey;
    //int pkeyLen;
    //unsigned char *ucBuf, *uctempBuf;

    server_cert = SSL_get_peer_certificate (sslx);
    CHK_NULL(server_cert);
    printf ("Server certificate:\n");

    str = X509_NAME_oneline (X509_get_subject_name (server_cert),0,0);
    CHK_NULL(str);
    printf ("\t subject: %s\n", str);
    OPENSSL_free (str);

    //str = X509_NAME_oneline (X509_get_issuer_name  (server_cert),0,0);
    //CHK_NULL(str);
    //printf ("\t issuer: %s\n", str);
    //OPENSSL_free (str);

    /* We could do all sorts of certificate verification stuff here before
        deallocating the certificate. */

    X509_free (server_cert);
}

int writeSSLSocket(SSL *sslx, char *msg, uint16_t size)
{
    int err = 0;
    /* --------------------------------------------------- */
    /* DATA EXCHANGE - Send a message and receive a reply. */
    err = SSL_write (sslx, msg, size);
    CHK_SSL(err);

    return err;
}

int readSSLSocket(SSL *sslx)
{
    memset(buf, 0, sizeof(buf));
    int err = SSL_read (sslx, buf, sizeof(buf) - 1);
    CHK_SSL(err);
    //buf[err] = '\0';
    return err;
}

int openSSLSocket()
{
    int err = 0;
    struct hostent *host_entry;
    struct sockaddr_in sa;
    const SSL_METHOD *meth = TLS_client_method();

    OpenSSL_add_ssl_algorithms();
    SSL_load_error_strings();
    ctx = SSL_CTX_new (meth);
    CHK_NULL(ctx);

    /* ----------------------------------------------- */
    /* Create a socket and connect to server using normal socket calls. */

    sd = socket (AF_INET, SOCK_STREAM, 0);
    CHK_ERR(sd, "socket");

    memset(&sa, 0, sizeof(sa));
    host_entry = gethostbyname(_apiHost.c_str());
    sa.sin_family      = AF_INET;
    sa.sin_addr        = *((struct in_addr *)host_entry->h_addr);
    sa.sin_port        = htons(_apiPort);

    err = connect(sd, (struct sockaddr*) &sa,
        sizeof(sa));
    CHK_ERR(err, "connect");

    /* ----------------------------------------------- */
    /* Now we have TCP conncetion. Start SSL negotiation. */
    ssl = SSL_new (ctx);
    CHK_NULL(ssl);    
    SSL_set_fd (ssl, sd);
    err = SSL_connect (ssl);
    CHK_SSL(err);

    /* Get the cipher - opt */
    printf ("SSL connection using %s\n", SSL_get_cipher (ssl));

    /* Get server's certificate (note: beware of dynamic allocation) - opt */
    //printfCertInfo(ssl);

    return err;
}

void transmit(ProtoMessage msg)
{
    string msgStr;
    unsigned char sizeArray[4];
    int size = msg.ByteSize();
    unsigned char *pkt = new unsigned char [size];
    //google::protobuf::io::ArrayOutputStream aos(pkt,size);
    msg.SerializeToArray(pkt, size);
    //
    printf("Msg(%u): ", size);
    for(int i = 0; i < 10; i++) {
        printf("%u ", pkt[i]);
    }
    cout << "\n";
    //
    size = htonl(size);
    memcpy(sizeArray, &size, 4);
    //
    writeSSLSocket(ssl, (char*)sizeArray, 4);
    writeSSLSocket(ssl, (char*)pkt, msg.ByteSize());
}

void authorizeApplication()
{
    OpenApiMessagesFactory msgFactory;
    ProtoMessage msg = msgFactory.CreateAppAuthorizationRequest(clientId,
                        clientSecret);
    transmit(msg);
}

void authorizeAccount()
{
    OpenApiMessagesFactory msgFactory;
    ProtoMessage msg = msgFactory.CreateAccAuthorizationRequest(accessToken,
                        _accountID);
    transmit(msg);
}

void getAccountsList()
{
    OpenApiMessagesFactory msgFactory;
    ProtoMessage msg = msgFactory.CreateAccountListRequest(accessToken);
    transmit(msg);
    // Wait for response
    //pthread_mutex_lock(&mutex_resp);
    //pthread_cond_wait(&wait_resp, &mutex_resp);
    //pthread_mutex_unlock(&mutex_resp);
}

void *read_task(void *arg)
{
    int ret = 0;
    int lenght, num;
    int msgType = 0;
    OpenApiMessagesFactory msgFactory;
    ProtoMessage protoMessage;
    ProtoOAGetAccountListByAccessTokenRes _accounts_list;
    while (1)
    {
        lenght = 0;
        ret = readSSLSocket(ssl);
        if (ret > 0) {
            memcpy(&num, buf, 4);
            // big endian to little endian
            lenght = ntohl(num);
            printf("lenght: %d\n", lenght);
        }
        if (lenght > 0) {
            string _message(buf+4);
            protoMessage = msgFactory.GetMessage(_message);
            msgType = protoMessage.payloadtype();
            cout << "MsgType: " << msgType << endl;
            //
            switch (msgType)
            {
                case PROTO_OA_APPLICATION_AUTH_RES:
                    cout << "App Auth res\n";
                    break;

                case PROTO_OA_ACCOUNT_AUTH_RES:
                    cout << "Acc Auth res\n";
                    break;

                case PROTO_OA_TRADER_RES:
                    cout << "TraderResp\n";
                    //var trader = ProtoOATraderRes.CreateBuilder().MergeFrom(protoMessage.Payload).Build();
                    //_traders.Add(trader.Trader);
                    break;

                case PROTO_OA_EXECUTION_EVENT:
                    cout << "Event\n";
                    /*var _payload_msg = msgFactory.GetExecutionEvent(_message);
                    if (_payload_msg.HasOrder)
                    {
                        testOrderId = _payload_msg.Order.OrderId;
                    }
                    if (_payload_msg.HasPosition)
                    {
                        testPositionId = _payload_msg.Position.PositionId;
                    }*/
                    break;

                case PROTO_OA_ERROR_RES:
                    cout << "Error\n";
                    cout << "Payload: " << msgFactory.getLastMessage() << endl;
                    break;

                case PROTO_OA_GET_ACCOUNTS_BY_ACCESS_TOKEN_RES:
                    cout << "Acc TokenRes\n";
                    cout << "Msg " << protoMessage.payload() << endl;
                    _accounts_list.MergeFrom(protoMessage);
                    for (int i = 0; i < _accounts_list.ctidtraderaccount_size(); i++) {
                        _accounts.push_back(_accounts_list.ctidtraderaccount(i));
                    }
                    break;

                default:
                    break;
            }
            //
        }
        usleep(100000);
    }
    pthread_exit(&ret);
}

int main(int argc, char* argv[])
{
    char opt;
    clientId = CLIID;
    clientSecret = CLSECRET;
    accessToken = ACCTOKEN;
    _accountID = ACCID;

    pthread_mutex_init(&mutex_resp, NULL);
    pthread_cond_init(&wait_resp, NULL);
    pthread_attr_init(&tattr);
    pthread_attr_setdetachstate(&tattr, PTHREAD_CREATE_DETACHED);
    //
    openSSLSocket();
    pthread_create(&thread_sslread, &tattr, read_task, NULL);
    while (1) {
        cout << "1- Authorize App\n2- getAccountsList\n3- Authorize Account\n";
        cin >> opt;
        switch(opt)
        {
            case '1':
                authorizeApplication();
                break;
            case '2':
                getAccountsList();
                break;
            case '3':
                authorizeAccount();
                break;
        }
    }

    SSL_shutdown (ssl);  /* send SSL/TLS close_notify */
    /* Clean up. */
    close (sd);
    SSL_free (ssl);
    SSL_CTX_free (ctx);

    return 0;
}
