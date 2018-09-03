#include "httpclientprivate.h"

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
    _path = path;
    _port = port;

    // Initing address
    if( !initAddress( path, port ) )
    {
        cerr << "initing address was failed." << endl;
        return false;
    }

    // Initing socket
    if( !initSocket() )
    {
        cerr << "initing socket was failed." << endl;
        return false;
    }

    cout << "successfully connected to " << path << ":" << port << "." << endl;
    return true;
}

bool HttpClientPrivate::downloadImage(const std::string &filename)
{
    std::stringstream request; // response storage
    request << "GET /" << filename << " HTTP/1.1\r\n"
            << "Host: " << _path << ":" << _port << "\r\n"
            << "Connection: keep-alive\r\n"
            << "Accept: */*\r\n";

    int result = send( connect_socket, request.str().c_str(),
                       request.str().length(), 0 );

    if( result == SOCKET_ERROR )
    {
        cerr << "send failed: " << WSAGetLastError() << endl;

        close();
        return false;
    }

    result = shutdown( connect_socket, SD_SEND );
    if( result == SOCKET_ERROR )
    {
        cerr << "shutdown failed. Code: " << WSAGetLastError() << endl;

        close();
        return false;
    }

    const int buffer_size = 1024;
    char buffer[ buffer_size ];

    std::vector<char> vBuffer;
    std::stringstream response;
    do
    {
        result = recv( connect_socket, buffer, buffer_size, 0 );

        if( result < 0 )
        {
            cerr << "error receiving data. Code: " << WSAGetLastError() << endl;
        }
        else if( result == 0 )
        {
            cout << "connection closed." << endl;
        }
        else
        {
            char *endPos = buffer + result;
            vBuffer.insert( vBuffer.end(), buffer, endPos );
            response.write( buffer, result );
        }
    }
    while( result > 0 );

    close();

    Response resp( vBuffer );

    if( resp.isOk() )
    {
        return saveFile( resp.getFilename(), resp.body );
    }
    else
    {
        cerr << "response error." << endl;
        return false;
    }
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

bool HttpClientPrivate::saveFile(const std::string &filename, const vector<char> &data)
{
    if( data.empty() )
    {
        cerr << "data is empty" << endl;
        return false;
    }
    std::ofstream out( filename, std::ios::out | std::ios::binary );

    if( !out.is_open() )
    {
        cerr << "error opening file: " << filename << endl;
        return false;
    }

    out.write( (const char*)&data[ 0 ], data.size() );
    out.close();

    cout << "file " << filename << " saved." << endl;
    return true;
}

void HttpClientPrivate::close()
{
    if( addr ) freeaddrinfo( addr );
    if( connect_socket != 0 ) closesocket( connect_socket );
    WSACleanup(); // uploading ws2_32.dll
}

vector<vector<char> > HttpClientPrivate::cutHead(vector<char> &src)
{
    vector< vector< char > > result;
    auto rowStartPos = src.begin();

    for( auto i = src.begin(); i != src.end(); ++i )
    {
        // Check on LF
        if( *i == '\n' )
        {
            // Check on CR
            if(    ( i != src.begin() )
                && ( *(i - 1) == '\r' ) )
            {
                // End of the line founded

                // Check empty row case (and starting of response-body)
                if( rowStartPos != (i - 1) )
                {
                    // pushing all row to result without '\r\n'
                    result.push_back( vector<char>( rowStartPos, i-1 ) );

                    // move pointer to next row beginnig point
                    if( i != (src.end()-1) )
                    {
                        rowStartPos = i + 1;
                    }
                }
                else
                {
                    src.erase( src.begin(), i+1 );
                    return result;
                }
            }
        }

        if( i == (src.end()-1) )
        {
            // Erase all data, no body founded
            src.erase( src.begin(), i+1 );
            return result;
        }
    }

    return result;
}

vector<std::string> HttpClientPrivate::split(const string &str, const char s)
{
    stringstream buf(str);
    vector<string> result;
    string segment;

    while(std::getline(buf, segment, s))
    {
       result.push_back( segment );
    }
    return result;
}

HttpClientPrivate::Response::Response(const vector<char> &src)
{
    body = src;
    head = cutHead( body );

    if(head.empty())
    {
        cerr << "response head is empty" << endl ;
        return;
    }

    // Parse first row
    string row1( head.front().begin(), head.front().end());

    vector<string> row1Parts = split( row1, ' ' );
    if( row1Parts.size() < 3 )
    {
        cerr << "Http head error: to few arguments in 1st row." << endl;
        statusCode   = string();
        reasonPhrase = string();
        return;
    }
    else
    {
        statusCode   = row1Parts[ 1 ];

        reasonPhrase = "";
        for( int i = 2; i < row1Parts.size(); ++i )
        {
            reasonPhrase += row1Parts[ i ];
        }
    }

    for( int i = 1; i < head.size(); ++i )
    {
        string row( head[ i ].begin(), head[ i ].end() );
        vector< string > rowParts = split( row, ':' );

        if( rowParts.size() < 2 )
        {
            cerr << "error param row" << endl;
            continue;
        }

        properties[ rowParts[ 0 ] ] = rowParts[ 1 ];
    }
}

bool HttpClientPrivate::Response::isOk() const
{
    if( statusCode.find( "200" ) != std::string::npos )
    {
        return true;
    }
    else
    {
        return false;
    }
}

std::string HttpClientPrivate::Response::getStatusCode() const
{
    return statusCode;
}

std::string HttpClientPrivate::Response::getReasonPhrase() const
{
    return reasonPhrase;
}

std::string HttpClientPrivate::Response::getFilename() const
{
    // Searching field with filename data
    if( !isKeyInMap( "Content-Disposition", properties ) )
    {
        cerr << "field with filename not found." << endl;
        return string();
    }
    string row = properties.at( "Content-Disposition" );

    // Searching filename parameter
    std::size_t startPos = row.find( "filename=\"" );
    if( startPos == std::string::npos )
    {
        cerr << "filename parameter not found." << endl;
        return string();
    }

    startPos += 10; // shift to end of "filename=""

    // Searching end of filename parameter
    std::size_t endPos = row.find( "\"", startPos );
    if( endPos == std::string::npos )
    {
        cerr << "filename parameter setted not correct." << endl;
        return string();
    }

    // Extracting filename
    return string( &row[ startPos ], &row[ endPos ] );
}

bool HttpClientPrivate::Response::isKeyInMap(const std::string &k, const std::map<std::string, std::string> &m) const
{
    auto itFound = m.find(k);
    return itFound != m.end();
}
