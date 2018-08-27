#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

#include <string>

using std::string;

class HttpClientPrivate;

class HttpClient
{
public:
    HttpClient(){}

    bool connect( const string &path, const string &port );
    bool getFile( const string &filename );

private:
    HttpClientPrivate *client;
};

#endif // HTTPCLIENT_H
