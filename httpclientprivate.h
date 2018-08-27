#ifndef HTTPCLIENTPRIVATE_H
#define HTTPCLIENTPRIVATE_H

#include <string>

#include <winsock2.h>
#include <ws2tcpip.h>

using std::string;

class HttpClientPrivate
{
public:
    HttpClientPrivate(): connect_socket(0), addr(nullptr){}

    bool connectToServer( const string &path, const string &port );
    bool downloadImage( const string &filename );

private:
    bool initAddress( const string &path, const string &port );

    bool initSocket();

    void close();

    SOCKET connect_socket;
    struct addrinfo *addr;
};

#endif // HTTPCLIENTPRIVATE_H
