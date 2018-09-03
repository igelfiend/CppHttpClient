#ifndef HTTPCLIENTPRIVATE_H
#define HTTPCLIENTPRIVATE_H

#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <iostream>
#include <algorithm> // for copy
#include <iterator> // for ostream_iterator

#include <winsock2.h>
#include <ws2tcpip.h>

using std::string;
using std::stringstream;
using std::vector;

/**
 * @brief The HttpClientPrivate class provides api for HTTP client.
 *        It can connect to server and request for downloading image
 *        on server.
 */
class HttpClientPrivate
{
public:
    HttpClientPrivate(): connect_socket(0), addr(nullptr){}

    /**
     * @brief connectToServer - connects to server at path:port.
     * @param path - path of the server.
     * @param port - port of the server.
     * @return true if success.
     */
    bool connectToServer( const string &path, const string &port );

    /**
     * @brief downloadImage - requesting downloading image from server.
     * @param filename - name of file for downloading.
     * @return true if success.
     */
    bool downloadImage( const string &filename );

private:

    /**
     * @brief The Response class parse response data
     *        and provide data access from response
     *        in more usable form.
     */
    class Response
    {
    public:
        explicit Response( const vector<char> &src );

        /**
         * @brief isOk - received 200 http code.
         * @return true if received 200.
         */
        bool isOk() const;

        /**
         * @brief getStatusCode - getter for status code value.
         * @return status code.
         */
        string getStatusCode() const;

        /**
         * @brief getReasonPhrase - getter for reason phrase/
         * @return reason phrase.
         */
        string getReasonPhrase() const;

        /**
         * @brief getFilename - extracts filename from response.
         * @return return empty string if failed.
         */
        string getFilename() const;

        string statusCode;
        string reasonPhrase;

        /**
         * @brief properties - stores fields of http head.
         */
        std::map<string, string> properties;

        /**
         * @brief head - contains head rows.
         */
        vector< vector< char > > head;

        /**
         * @brief body - contains response body.
         */
        vector< char > body;

    private:
        /**
         * @brief isKeyInMap - service function for map. Checks if key presents in map.
         * @param k - key value.
         * @param m - checked map.
         * @return true if map contains the key.
         */
        bool isKeyInMap(const string& k, const std::map<string,string>& m) const;
    };

    /**
     * @brief initAddress - inits address for client.
     * @param path - init path.
     * @param port - init port.
     * @return true on success.
     */
    bool initAddress( const string &path, const string &port );

    /**
     * @brief initSocket - init socket for client.
     * @return true on success.
     */
    bool initSocket();


    /**
     * @brief saveFile - Saving file with filename, using data as file content.
     * @param filename - name of saving file.
     * @param data - file content.
     * @return true on success.
     */
    bool saveFile(const string &filename, const vector<char> &data );

    /**
     * @brief close - free all data.
     */
    void close();

    /**
     * @brief cutHead - cut HTTP head from vector<char> data into vector of vectors,
     * @brief           and clear all head-data from source.
     * @param src - source http response data
     * @return vector of vectors<char>, contains reponse head lines
     */
    static vector< vector<char>> cutHead(vector<char> &src);

    /**
     * @brief split - service function for splitting string to vector of strings.
     * @param str - source string.
     * @param s - separator.
     * @return vector of string, contants parts of str.
     */
    static vector< string > split( const string &str, const char s );

    /**
     * @brief connect_socket - socket for working with server.
     */
    SOCKET connect_socket;

    /**
     * @brief addr - Storing information about client IP data.
     */
    struct addrinfo *addr;

    /**
     * @brief _path - connecting path.
     */
    string _path;

    /**
     * @brief _port - connecting port.
     */
    string _port;
};

#endif // HTTPCLIENTPRIVATE_H
