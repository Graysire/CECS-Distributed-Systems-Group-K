// Group K Server.cpp : This file contains the 'main' function. Program execution begins and ends there.
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

        std::cout << readFromSocket(socket) << std::endl;
        std::cout << readFromSocketUntil(socket, '\r') << std::endl;

    }
    catch (system::system_error& e) {
        std::cout << "Error occured! Error code = " << e.code()
            << ". Message: " << e.what();

        return e.code().value();
    }

    return 0;
}