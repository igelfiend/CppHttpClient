#include "httpclient.h"

#include "httpclientprivate.h"

HttpClient::~HttpClient()
{
    delete client;
}

bool HttpClient::connect(const std::string &path, const std::string &port)
{
    if( client )
    {
        delete client;
    }

    client = new HttpClientPrivate();
    return client->connectToServer( path, port );
}

bool HttpClient::downloadFile(const std::string &filename)
{
    if( !client ) return false;

    return client->downloadImage( filename );
}
