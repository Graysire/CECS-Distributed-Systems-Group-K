// Group K Server.cpp : This file contains the 'main' function. Program execution begins and ends there.
//


#include <iostream>
#include <boost/asio.hpp>

using namespace boost;

int main()
{
    //https://subscription.packtpub.com/book/application_development/9781783986545/1/ch01lvl1sec16/accepting-connections

    // The size of the queue containing the pending connection requests.
    const int BACKLOG_SIZE = 30;

    //this is the prot that will be listened to
    unsigned short listenPort = 3333;

    // Creates the endpoint for use by the socket(s) for communication using TCP
    asio::ip::tcp::endpoint serverEndpoint(asio::ip::address_v4::any(), listenPort);

    // I/O context that is used as a basis for communicating I/O commands to the OS
    asio::io_context ios;

    //Attempt to create the connection
    try {
        // Step 3. Instantiating and opening an acceptor socket.
        //this will be used to listen for incoming connections
        asio::ip::tcp::acceptor acceptor(ios, serverEndpoint.protocol());

        // Step 4. Binding the acceptor socket to the server endpoint
        // Connect the acceptorand the endpoint to allow the acceptor to listen for connection requests
        acceptor.bind(serverEndpoint);

        // Step 5. Starting to listen for incoming connection requests
        //Listen for any incoming requests with a maximum queue of BACKLOG_SIZE
        acceptor.listen(BACKLOG_SIZE);

        // Step 6. Creating an active socket.
        //Create an active socket to connect to any communication requests received
        asio::ip::tcp::socket socket(ios);

        // Step 7. Processing the next connection request and connecting the active socket to the client.
        //block until we receive a request, then connect the socket ot the request
        acceptor.accept(socket);

        std::cout << "accepted" << std::endl;

        // At this point 'sock' socket is connected to 
        // the client application and can be used to send data to
        // or receive data from it.
    }
    catch (system::system_error& e) {
        std::cout << "Error occured! Error code = " << e.code()
            << ". Message: " << e.what();

        return e.code().value();
    }

    return 0;
}