// Group K Client.cpp : This file contains the 'main' function. Program execution begins and ends there.
//


#include <iostream>
#include <boost/asio.hpp>
#include <vector>
#include <stdio.h>

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

//Function to read a string from a socket
std::string readFromSocket(asio::ip::tcp::socket& socket) 
{
    //(maximum?) number of bytes of the expected message
    const unsigned char MESSAGE_SIZE = 5;
    //initialize variable to receive the message
    char message[MESSAGE_SIZE];

    //read the full message of size MESSAGE_SIZE into message
    asio::read(socket, asio::buffer(message, MESSAGE_SIZE));

    //return message as a string
    return std::string(message, MESSAGE_SIZE);

    //As with write, this is the complex form of the code handled by read()
    //number of bytes received so far
    //std::size_t bytesRead = 0;

    ////keep running loop until all expected bytes have been received
    //while (bytesRead != MESSAGE_SIZE) 
    //{
    //    bytesRead += socket.read_some(asio::buffer(message + bytesRead, MESSAGE_SIZE - bytesRead));
    //}

    ////convert to string and return
    //return std::string(message, bytesRead);
}

//Function to read a string from a socket, ending after reaching a delimiter character
std::string readFromSocketUntil(asio::ip::tcp::socket& socket, char delim)
{
    //declare stream buffer
    asio::streambuf sBuffer;

    //Synchronously read data from socket until delim character is encountered
    asio::read_until(socket, sBuffer, delim);

    //declare message
    std::string message;

    //extract message from the buffer
    std::istream input_stream(&sBuffer);
    std::getline(input_stream, message);
    //return the message
    return message;


}

//void do_send(asio::ip::udp::socket socket)
//{
//
//    std::ostringstream os;
//    os << "Message";
//    std::string message_ = os.str();
//
//    socket_.async_send_to(
//        boost::asio::buffer(message_), endpoint_,
//        (boost::system::error_code ec, std::size_t /*length*/)
//        {
//            if (!ec && message_count_ < max_message_count)
//                do_timeout();
//        });
//}

std::vector<std::string> findIP()
{
    std::vector<std::string> ipList;
    FILE* pipe = _popen("arp -a", "r");
    if (!pipe)
    {
        fprintf(stderr, "Unable to open IP Scan pipe");
    }
    else
    {
        char buffer[128];
        while (!feof(pipe))
        {
            if (fgets(buffer, 128, pipe) != NULL)
            {
                std::cout << buffer << std::endl;
            }
        }
        _pclose(pipe);
    }
   
    return ipList;

}


void handle(const boost::system::error_code ec, std::size_t t)
{}

int main()
{
    //https://subscription.packtpub.com/book/application_development/9781783986545/1/ch01lvl1sec15/connecting-a-socket

    // Step 1. Assume that the client application has already
    // obtained the IP address and protocol port number of the
    // target server.
    std::string targetRawIP = "239.255.0.1";
    unsigned short targetPort = 3333;

    findIP();

    try {
        // Step 2. Creating an endpoint designating  target server application.
        // this will be used by the sockets as an access point for communication using TCP
        asio::ip::udp::endpoint clientEndpoint(asio::ip::address::from_string(targetRawIP), targetPort);

        // I/O context that is used as a basis for communicating I/O commands to the OS
        asio::io_context ios;

        // Step 3. Creating and opening a socket.
        // this will allow the socket to be used
        asio::ip::udp::socket socket(ios, clientEndpoint.protocol());

        // Step 4. Connecting a socket.
        // this will attemptto connect the socket to the server
        socket.connect(clientEndpoint);

        std::cout << "connected" << std::endl;

        boost::system::error_code ec;
        std::ostringstream os;
        os << "Message";
        std::string message_ = os.str();
        while (true)
        {

            socket.async_send_to(
                boost::asio::buffer(message_, 7), clientEndpoint,
                handle);
        }
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