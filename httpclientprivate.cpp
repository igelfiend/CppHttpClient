#include "httpclientprivate.h"

#include <iostream>
#include <sstream>

#include <fstream> // for reading file

#include <vector>

using std::cerr;
using std::cout;
using std::endl;

#define _WIN32_WINNT 0x501

// Links winsock2 lib
#pragma comment(lib, "Ws2_32.lib")

bool HttpClientPrivate::connectToServer(const std::string &path, const std::string &port)
{
    // Initing address
    if( !initAddress( path, port ) )
    {
        return false;
    }

    // Initing socket
    if( !initSocket() )
    {
        return false;
    }

    return true;
}

bool HttpClientPrivate::downloadImage(const std::string &filename)
{
    std::stringstream request; // response storage
    request << "GET /cat.jpg HTTP/1.1\r\n"
            << "Host: " << "127.0.0.1:8000" << "\r\n"
            << "Connection: keep-alive\r\n"
            << "Accept: */*\r\n";

    int result = send( connect_socket, request.str().c_str(), request.str().length(), 0 );

    if( result == SOCKET_ERROR )
    {
        cerr << "send failed: " << WSAGetLastError() << endl;

        close();
        return false;
    }

    return true;
}

bool HttpClientPrivate::initAddress(const std::string &path, const std::string &port)
{
    WSADATA wsaData; // Struct for store service information

    // Uploading ws2_32.dll
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);

    if (result != 0)
    {
        cerr << "WSAStartup failed: " << result << endl;
        return false;
    }

    addr = nullptr;

    // initing address struct
    struct addrinfo hints;
    ZeroMemory(&hints, sizeof(hints));

    hints.ai_family   = AF_UNSPEC;      // AF_INET select web for working with socket
    hints.ai_socktype = SOCK_STREAM;    // Stream type socket
    hints.ai_protocol = IPPROTO_TCP;    // Using TCP/IP protocol

    // Initing address struct
    result = getaddrinfo(path.c_str(), port.c_str(), &hints, &addr);

    // On error close all
    if (result != 0)
    {
        cerr << "getaddrinfo failed: " << result << endl;
        close();
        return false;
    }

    return true;
}

bool HttpClientPrivate::initSocket()
{
    // Creating socket
    connect_socket = socket( addr->ai_family,
                            addr->ai_socktype,
                            addr->ai_protocol );

    // On error free memory
    if(connect_socket == INVALID_SOCKET)
    {
        cerr << "Error at socket: " << WSAGetLastError() << endl;
        connect_socket = 0;

        close();
        return false;
    }

    // Binding socket to IP-address
    int result = connect( connect_socket,
                          addr->ai_addr,
                          int( addr->ai_addrlen ) );

    // If binding failed, free memory
    if (result == SOCKET_ERROR)
    {
        cerr << "connect failed with error: " << WSAGetLastError() << endl;

        close();
        return false;
    }

    if( connect_socket == INVALID_SOCKET )
    {
        cerr << "Unable to connect to server!" << endl;
        close();
        return false;
    }

    return true;
}

void HttpClientPrivate::close()
{
    if( addr ) freeaddrinfo( addr );
    if( connect_socket != 0 ) closesocket( connect_socket );
    WSACleanup(); // uploading ws2_32.dll
}
