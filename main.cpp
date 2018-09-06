#include <iostream>
#include <string>
#include "httpclient.h"

using namespace std;

int main(int argc, char *argv[])
{
    if( argc < 4 )
    {
        cerr << "to few arguments count" << endl;
        return -1;
    }

    string strAddres( argv[ 1 ] );
    string strPort  ( argv[ 2 ] );
    string filename ( argv[ 3 ] );

    HttpClient client;
    if( !client.connect( strAddres, strPort ) )
    {
        cerr << "connect error. Aborting." << endl;
        return -1;
    }

    if( !client.downloadFile( filename ) )
    {
        cerr << "error downloading file" << endl;
        return -2;
    }

    return 0;
}
