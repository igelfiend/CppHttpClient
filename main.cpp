#include <iostream>
#include "httpclient.h"

using namespace std;

int main()
{
    HttpClient client;
    if( !client.connect( "127.0.0.1", "8000" ) )
    {
        std::cerr << "Error connecting to server" << std::endl;
    }
    else
    {
        cout << "Successfully connected" << std::endl;
//        client.getFile( "cat.jpg" );
    }


    return 0;
}
