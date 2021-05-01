// Group K File Synchronization.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <boost/asio.hpp>
#include <boost/array.hpp>

using namespace boost;
using boost::asio::ip::tcp;

const unsigned short PORT = 3333;

//writes message to a socket synchronously
void writeToSocket(asio::ip::tcp::socket& socket, std::string message)
{
    std::size_t bytesWritten = 0;

    while (bytesWritten != message.length())
    {
        bytesWritten += socket.write_some(asio::buffer(message.c_str() + bytesWritten, message.length() - bytesWritten));
    }
}

std::vector<std::string> findIP()
{
    //create a vector to store the IPs in
    std::vector<std::string> ipList;
    //Open a command prompt and use arp -a to retrieve a list of IP addresses and hteir interfaces
    FILE* pipe = _popen("arp -a", "r");
    //If the pupe was not created print the error
    if (!pipe)
    {
        fprintf(stderr, "Unable to open IP Scan pipe");
    }
    //if the pipe was created continue
    else
    {
        //instantiate buffer and string
        char buffer[128];
        std::string bufferString = "";

        //until the pipe contains no more data add the data to the string using the buffer
        while (!feof(pipe))
        {
            if (fgets(buffer, 128, pipe) != NULL)
            {
                bufferString += buffer;
            }
        }
        //Close the pipe
        _pclose(pipe);

        //std::cout << bufferString;
        //create int for tracking index in string
        int startIndex = 0;
        //iterate through bufferString
        for (int i = 0; i < bufferString.size(); i++)
        {
            //if the current value is an integer or a '.' continue
            if (isdigit(bufferString[i]) || bufferString[i] == '.')
            {
                continue;
            }
            //if a space is found and the discovered string is of sufficient length to be an IP, add it to the vector
            else if (bufferString[i] == ' ' && i - startIndex > 3)
            {
                ipList.push_back(bufferString.substr(startIndex + 1, i - startIndex - 1));
            }
            //set start index to the current index
            startIndex = i;
        }

    }


    return ipList;

}

//Searches the local subnet/vLAN for IP addresses and attempts to connect to each one
//Creates a socket and adds it to the socketVector for each connection
void findAndConnect(std::vector<asio::ip::tcp::socket> &socketVector, asio::io_context &ios)
{
    //create a vector to store the IPs in
    std::vector<std::string> ipList = findIP();

    for (int i = 0; i < ipList.size(); i++)
    {
        try
        {
            // Create an endpoint designating target server application.
            // this will be used by the sockets as an access point for communication using TCP
            std::cout << "Attempting to Connect to " << ipList[i] << "...";
            asio::ip::tcp::endpoint endpoint(asio::ip::address::from_string(ipList[i]), PORT);

            //Create a socket attached to theendpoint
            asio::ip::tcp::socket socket(ios, endpoint.protocol());

            //Attempt to connect the socket to the endpoint
            socket.connect(endpoint);

            std::cout << "Connected" << std::endl;
        }
        catch (system::system_error& e) 
        {
            std::cout << "Connection Failed " << e.what() << std::endl;
            //std::cout << "Error occured! Error code = " << e.code()
            //    << ". Message: " << e.what() << std::endl;;

            //return e.code().value();
        }
    }


}


int main()
{
    //Creates the I/O context to be used as a basis for communicating I/O commands to the OS
    asio::io_context ios;

    //create vector to hold the active sockets
    std::vector<asio::ip::tcp::socket> socketVector;

    findAndConnect(socketVector, ios);

}
