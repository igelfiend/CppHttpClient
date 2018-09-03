#include <iostream>
#include "httpclient.h"

using namespace std;

int main()
{
    HttpClient client;
    if( client.connect( "127.0.0.1", "8000" ) )
    {
        client.downloadFile( "cat.jpg" );
    }

    return 0;
}
