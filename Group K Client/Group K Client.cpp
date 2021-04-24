// Group K Client.cpp : This file contains the 'main' function. Program execution begins and ends there.
//


#include <iostream>
#include <boost/asio.hpp>

using namespace boost;

//Function to write the message to the socket
void writeToSocket(asio::ip::tcp::socket& socket, std::string message)
{
    //this writes the entire buffer to the socket
    //it works approximately the same way as the below loop
    asio::write(socket, asio::buffer(message));

    //this is a needlessly complex form of writing, but explaisn how the writing works
    // Declare size of number of bytes that have been written
    //std::size_t bytesWritten = 0;

    //// Run the loop until all data is written to the socket.
    //while (bytesWritten != message.length())
    //{
    //    //write bytes and increment the number of bytes that have been written
    //    bytesWritten += socket.write_some(asio::buffer(message.c_str() + bytesWritten, message.length() - bytesWritten));
    //}
}




int main()
{
    //https://subscription.packtpub.com/book/application_development/9781783986545/1/ch01lvl1sec15/connecting-a-socket

    // Step 1. Assume that the client application has already
    // obtained the IP address and protocol port number of the
    // target server.
    std::string targetRawIP = "127.0.0.1";
    unsigned short targetPort = 3333;

    try {
        // Step 2. Creating an endpoint designating  target server application.
        // this will be used by the sockets as an access point for communication using TCP
        asio::ip::tcp::endpoint clientEndpoint(asio::ip::address::from_string(targetRawIP), targetPort);

        // I/O context that is used as a basis for communicating I/O commands to the OS
        asio::io_context ios;

        // Step 3. Creating and opening a socket.
        // this will allow the socket to be used
        asio::ip::tcp::socket socket(ios, clientEndpoint.protocol());

        // Step 4. Connecting a socket.
        // this will attemptto connect the socket to the server
        socket.connect(clientEndpoint);

        std::cout << "connected" << std::endl;

        // At this point socket 'sock' is connected to 
        // the server application and can be used
        // to send data to or receive data from it.

        writeToSocket(socket, "Hello");




    }
    // Overloads of asio::ip::address::from_string() and 
    // asio::ip::tcp::socket::connect() used here throw
    // exceptions in case of error condition.
    catch (system::system_error& e) {
        std::cout << "Error occured! Error code = " << e.code()
            << ". Message: " << e.what();

        return e.code().value();
    }

    return 0;
}