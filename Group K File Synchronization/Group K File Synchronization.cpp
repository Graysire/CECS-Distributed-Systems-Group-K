// Group K File Synchronization.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <boost/asio.hpp>
#include <boost/array.hpp>

using namespace boost;
using boost::asio::ip::tcp;

//writes message to a socket synchronously
void writeToSocket(asio::ip::tcp::socket& socket, std::string message)
{
    std::size_t bytesWritten = 0;

    while (bytesWritten != message.length())
    {
        bytesWritten += socket.write_some(asio::buffer(message.c_str() + bytesWritten, message.length() - bytesWritten));
    }
}



int main()
{
    //NOTE this portion of the code is taken primarily from the following tutorial
    //this serves as an introduction to boost and its libraries
    //however all comments are based on my (Grayson) understanding on the code therein and its use
    //as a part of our overall project
    //https://subscription.packtpub.com/book/application_development/9781783986545/1/ch01lvl1sec11/creating-an-active-socket

    ////Step 1: Create an io_service object
    ////this provides network I/O services
    ////it is required for all sockets and is central to our network
    //asio::io_context ioContext;

    ////Step 2: Create a tcp object
    ////Acts as a data structure containing values for the TCP protocol
    ////replace 'v4()' with 'v6()' to change to using IPv6
    //asio::ip::tcp connectionProtocol = asio::ip::tcp::v4();

    ////Step 3: Create a socket
    ////Instantiates a socket using our ioService
    ////This socket is unusable until after it has been opened
    //asio::ip::tcp::socket socket(ioContext);

    ////Step 3.5: Create error code object
    ////this will store any error that occurs while opening the socket
    //boost::system::error_code errorCode;

    ////Step 4: Open the socket
    ////This will allow the socket to communicate with other sockets
    //socket.open(connectionProtocol, errorCode);

    ////If an error is received, print said error and return
    //if (errorCode.value() != 0)
    //{
    //    std::cout << "Failed to open socket, Error code = " << errorCode.value() << ". Message " << errorCode.message();
    //    return errorCode.value();
    //}

    //Size of queue containing the pending connection requests
    const int BACKLOG_SIZE = 30;


    //Establish the I/O interaction context that allows I/O access
    asio::io_context ioContext;
    //Establish Connection Protocol to use
    asio::ip::tcp connectionProtocol = asio::ip::tcp::v6();
    //Create an object representing all available IPv6 Addresses available to this program
    asio::ip::address ipAddress = asio::ip::address_v6::any();
    //Port we intend to listen with
    int listenPort = 1998;


    //Create an endpoint at our address on our listening port
    asio::ip::tcp::endpoint endpointServer(ipAddress, listenPort);


    //Create an acceptor socket to listen with
    asio::ip::tcp::acceptor listenSocket(ioContext, connectionProtocol);
    //Declare error code
    boost::system::error_code error;

    //Bind the listening endpoint to the acceptor socket
    listenSocket.bind(endpointServer, error);

    //Sets the acceptor to listen
    listenSocket.listen(BACKLOG_SIZE);

    //create an active socket for the server to use
    asio::ip::tcp::socket serverSocket(ioContext);

    if (error.value() != 0)
    {
    std::cout << "Failed to open socket, Error code = " << error.value()
        << ". Message " << error.message();
    return error.value();
    }

    //client perspective connection
    //should target the server's address and port
    asio::ip::tcp::endpoint endpointClient(ipAddress, listenPort);

    //create an active socket to connect with
    asio::ip::tcp::socket clientSocket(ioContext, connectionProtocol);

    //connect the socket to the endpoint
    clientSocket.connect(endpointClient);

    //attempt to accept pending request
    listenSocket.accept(serverSocket);

    if (error.value() != 0)
    {
        std::cout << "Failed to open socket, Error code = " << error.value()
            << ". Message " << error.message();
        return error.value();
    }


}
