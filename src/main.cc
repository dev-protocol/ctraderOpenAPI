#include <iostream>
#include <fstream>
#include <string>
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
//IList<ProtoOACtidTraderAccount> _accounts;
//List<ProtoOATrader> _traders;

// SSL vars
int sd;
SSL_CTX* ctx;
SSL*     ssl;
char buf[4096];

void printfCertInfo(SSL *sslx)
{
    char*    str;
    X509*    server_cert;
    server_cert = SSL_get_peer_certificate (sslx);
    CHK_NULL(server_cert);
    printf ("Server certificate:\n");

    str = X509_NAME_oneline (X509_get_subject_name (server_cert),0,0);
    CHK_NULL(str);
    printf ("\t subject: %s\n", str);
    OPENSSL_free (str);

    str = X509_NAME_oneline (X509_get_issuer_name  (server_cert),0,0);
    CHK_NULL(str);
    printf ("\t issuer: %s\n", str);
    OPENSSL_free (str);

    /* We could do all sorts of certificate verification stuff here before
        deallocating the certificate. */

    X509_free (server_cert);
}

int writeSSLSocket(SSL *sslx, string msg)
{
    int err = 0;
    /* --------------------------------------------------- */
    /* DATA EXCHANGE - Send a message and receive a reply. */
    err = SSL_write (sslx, msg.c_str(), strlen(msg.c_str()));
    CHK_SSL(err);

    return err;
}

int readSSLSocket(SSL *sslx)
{
    memset(buf, 0, sizeof(buf));
    int err = SSL_read (sslx, buf, sizeof(buf) - 1);
    CHK_SSL(err);
    //buf[err] = '\0';
    printf ("Got %d chars:'%s'\n", err, buf);
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

    /* Following two steps are optional and not required for
        data exchange to be successful. */

    /* Get the cipher - opt */

    printf ("SSL connection using %s\n", SSL_get_cipher (ssl));

    /* Get server's certificate (note: beware of dynamic allocation) - opt */
    printfCertInfo(ssl);

    return err;
}

void transmit(ProtoMessage msg)
{
    string msgStr;
    msg.SerializeToString(&msgStr);
    cout << "Msg: " << msgStr << endl;
    writeSSLSocket(ssl, msgStr);
}

void authorizeApplication()
{
    OpenApiMessagesFactory msgFactory;
    ProtoMessage msg = msgFactory.CreateAppAuthorizationRequest(clientId,
                        clientSecret);
    transmit(msg);
}

int main(int argc, char* argv[])
{
    char opt;
    clientId = CLIID;
    clientSecret = CLSECRET;
    accessToken = ACCTOKEN;
    _accountID = ACCID;

    openSSLSocket();
    while (1) {
        cout << "1 - Authorize App\n2 - Authorize Account\n";
        cin >> opt;
        switch(opt)
        {
            case '1':
                authorizeApplication();
                break;
        }
        readSSLSocket(ssl);
        readSSLSocket(ssl);
    }

    SSL_shutdown (ssl);  /* send SSL/TLS close_notify */
    /* Clean up. */
    close (sd);
    SSL_free (ssl);
    SSL_CTX_free (ctx);

    return 0;
}
