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

void handle(const boost::system::error_code ec, std::size_t t)
{}

void recieveHandler(const boost::system::error_code& error, std::size_t bytes_transferred) {
}

void recieveFileFromClient(asio::ip::tcp::socket& socket) {
    std::string fileDirectory = "file1.txt";
    const char* fileName = fileDirectory.c_str();
    FILE* fi;
    errno_t err;
    char buffer[100];
    if ((err = fopen_s(&fi, fileName, "w") != 0)) {
        std::cout << "Cannot open given file.";
        return;
    }
    int recievedBytes = 0;
    socket.async_read_some(boost::asio::buffer(&fi, 100), recieveHandler);
    //fputs(buffer, stdout);
    std::cout << "Receieved file?\n";
    fclose(fi);
}


//ARP proto
int main()
{
    //https://subscription.packtpub.com/book/application_development/9781783986545/1/ch01lvl1sec16/accepting-connections

    // The size of the queue containing the pending connection requests.
    const int BACKLOG_SIZE = 30;

    //this is the prot that will be listened to
    unsigned short listenPort = 3333;

    std::string listenIP = "0.0.0.0";
    boost::asio::ip::tcp::endpoint listen;

    // Creates the endpoint for use by the socket(s) for communication using TCP
    asio::ip::tcp::endpoint serverEndpoint(asio::ip::address::from_string(listenIP), listenPort);

    // I/O context that is used as a basis for communicating I/O commands to the OS
    asio::io_context ios;

    //Attempt to create the connection
    try 
    {
        boost::asio::ip::tcp::acceptor acceptor(ios, serverEndpoint.protocol());
        acceptor.bind(serverEndpoint);
        acceptor.listen(BACKLOG_SIZE);

        asio::ip::tcp::socket socket(ios);
        acceptor.accept(socket);

        //std::array<char, 1024> datra;
        std::cout << "Received\n";
        //std::cout << readFromSocketUntil(socket,'\n');
        recieveFileFromClient(socket);

        
        while (true) {}
        

        //while (true)
        //{
        //    try
        //    {
        //        std::cout << socket.remote_endpoint() << std::endl;
        //        //socket.async_receive_from(boost::asio::buffer(datra), listen, handle);
        //        //std::cout.write(datra.data(), 7);
        //    }
        //    catch(system::system_error&e)
        //    {

        //    }
        //}

    }
    catch (system::system_error& e) {
        std::cout << "Error occured! Error code = " << e.code()
            << ". Message: " << e.what();

        return e.code().value();
    }
    
    return 0;
}