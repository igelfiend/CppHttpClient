#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

#include <string>

using std::string;

class HttpClientPrivate;

/**
 * @brief The HttpClient class provides interface for Http client
 *        It has simple interface for connecting at path:port
 *        and downloading file from this server.
 */
class HttpClient
{
public:
    HttpClient(): client(nullptr){}
    ~HttpClient();

    /**
     * @brief connect - connects client to server at path:port.
     * @param path - path of the server.
     * @param port - port of the server.
     * @return true if success.
     */
    bool connect( const string &path, const string &port );

    /**
     * @brief downloadFile - request file data from server.
     * @param filename - filename on server.
     * @return true if success.
     */
    bool downloadFile( const string &filename );

private:
    /**
     * @brief client - api.
     */
    HttpClientPrivate *client;
};

#endif // HTTPCLIENT_H
