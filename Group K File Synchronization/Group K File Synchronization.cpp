// Group K File Synchronization.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <boost/asio.hpp>
#include <iostream>

using namespace boost;

int main()
{
    //NOTE this portion of the code is taken primarily from the following tutorial
    //this serves as an introduction to boost and its libraries
    //however all comments are based on my (Grayson) understanding on the code therein and its use
    //as a part of our overall project
    //https://subscription.packtpub.com/book/application_development/9781783986545/1/ch01lvl1sec11/creating-an-active-socket

    //Step 1: Create an io_service object
    //this provides network I/O services
    //it is required for all sockets and is central to our network
    asio::io_service ioService;

    //Step 2: Create a tcp object
    //Acts as a data structure containing values for the TCP protocol
    //replace 'v4()' with 'v6()' to change to using IPv6
    asio::ip::tcp connectionProtocol = asio::ip::tcp::v4();

    //Step 3: Create a socket
    //Instantiates a socket using our ioService
    //This sopcket is unusable until after it has been opened
    asio::ip::tcp::socket socket(ioService);

    //Step 3.5: Create error code object
    //this will store any error that occurs while opening the socket
    boost::system::error_code errorCode;

    //Step 4: Open the socket
    //This will allow the socket to communicate with other sockets
    socket.open(connectionProtocol, errorCode);
}
