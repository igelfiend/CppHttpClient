#include "httpclient.h"

#include "httpclientprivate.h"

bool HttpClient::connect(const std::string &path, const std::string &port)
{
    if( client )
    {
        delete client;
    }

    client = new HttpClientPrivate();
    return client->connectToServer( path, port );
}

bool HttpClient::getFile(const std::string &filename)
{
    if( !client ) return false;

    return client->downloadImage( filename );
}
