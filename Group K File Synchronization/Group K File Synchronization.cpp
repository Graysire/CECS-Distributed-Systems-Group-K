// Group K File Synchronization.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <string> 
#include <stdexcept>
#include <stdio.h>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/bind.hpp>

using namespace std;
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

        std::cout << bufferString;
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
            //if an x occurs, pop the last IP out, an x only appears in arp -a
            //after a reference to this computer, this will make it impossible to
            //connect to a different instance of this app on this machine
            else if (bufferString[i] == 'x')
            {
                ipList.pop_back();
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

std::vector<asio::ip::tcp::endpoint> findEndpoints()
{
    std::vector<asio::ip::tcp::endpoint> endpoints;

    //get the IPs
    std::vector<std::string> ipList = findIP();

    for (int i = 0; i < ipList.size(); i++)
    {
        asio::ip::tcp::endpoint ep(asio::ip::address::from_string(ipList[i]), PORT);
        endpoints.push_back(ep);
    }

    return endpoints;
}

class tcpConnection : public boost::enable_shared_from_this<tcpConnection>
{
public:
    typedef boost::shared_ptr<tcpConnection> pointer;

    static pointer create(asio::io_context& ios)
    {
        return pointer(new tcpConnection(ios));
    }

    asio::ip::tcp::socket& socket()
    {
        return socket_;
    }

    void startConnect()
    {
        //Do something on connecting to another app, like send or receive files

        //Example async write function
        /*boost::asio::async_write(socket_, boost::asio::buffer(message_),
            boost::bind(&tcp_connection::handle_write, shared_from_this(),
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));*/
    }
    void startAccept()
    {
        //Do something on being connected to by another app, like send or receive files

        //Example async write function
        /*boost::asio::async_write(socket_, boost::asio::buffer(message_),
            boost::bind(&tcp_connection::handle_write, shared_from_this(),
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));*/
    }


private:
    tcpConnection(asio::io_context& ios) : socket_(ios) {};

    //part of the async writing example above
    //void handle_write(const boost::system::error_code& /*error*/,
    //    size_t /*bytes_transferred*/)
    //{
    //}

    asio::ip::tcp::socket socket_;
};





//Class responsible for accepting connections
class tcpServer
{
public:
    //constructor which creates an acceptor and starts accepting connections
    tcpServer(asio::io_context& ios) : acceptor(ios, tcp::endpoint(tcp::v4(), PORT))
    {
        this->ios = &ios;
        startAccept();
        startConnect();
    }
private:
    //the first portion of acception a connection, starts accepting asynchronously
    void startAccept()
    {
        //create a connection object to represent the connection
        tcpConnection::pointer newConnection = tcpConnection::create(*ios);

        //accept the connection asynchronously and call handleAccept() when done
        acceptor.async_accept(newConnection->socket(), boost::bind(&tcpServer::handleAccept, this, newConnection, asio::placeholders::error));
    }

    //Handles the accpetance of a connection
    void handleAccept(tcpConnection::pointer newConnection, const system::error_code& error)
    {
        //if no error has occurred start the connection
        if (!error)
        {
            newConnection->startAccept();
        }

        //Go back to searching for more connections
        startAccept();
    }

    //Handles the success of a connection
    void handleConnect(tcpConnection::pointer newConnection, const system::error_code& error)
    {
        //if no error has occurred start the connection
        if (!error)
        {
            newConnection->startConnect();
        }

        //Go back to searching for more connections
        startConnect();
    }

    //the first portion of sending a connection asynchronously
    void startConnect()
    {
        //create a connection object to represent the connection
        tcpConnection::pointer newConnection = tcpConnection::create(*ios);

        //send connections asynchronously
        asio::async_connect(newConnection->socket(), findEndpoints(), boost::bind(&tcpServer::handleConnect, this, newConnection, asio::placeholders::error));
    }

    //the acceptor used to accept connections
    tcp::acceptor acceptor;
    asio::io_context* ios;
};

//obsolete file path locator code here
//string gainLoc(string fileName)
//{
//    char buffer[256];
//    string line = "";
//    string result = "";
//    string nameloc = "dir \"" + fileName + "*\" /s";
//    int pos = 0;
//    FILE* pipeName = _popen(nameloc.c_str(), "r");
//    if (!pipeName) {
//        return "popen1 in gainLoc failed";
//    }
//    while (!feof(pipeName)) {
//
//        // use buffer to read and add to result
//        if (fgets(buffer, 256, pipeName) != NULL)
//            line += buffer;
//    }
//
//    _pclose(pipeName);
//    pos = line.find("Directory of ");
//    result = line.substr(pos, line.find("\n",pos));
//    return result;
//
//}

//code which takes the output of the commandline after the call of file comparision 
string exec(std::string command) {
    char buffer[128];
    string result = "";

    // Open pipe to access the file in the system
    FILE* pipe = _popen(command.c_str(), "r");
    if (!pipe) {
        return "popen failed!";
    }

    // read till end of process of file compare
    while (!feof(pipe)) {

        // use buffer to read and add to result
        if (fgets(buffer, 128, pipe) != NULL)
            result += buffer;
    }

    _pclose(pipe);
    //the ending result can be long if files are not the same
    return result;
}

int main()
{
    //Creates the I/O context to be used as a basis for communicating I/O commands to the OS
    asio::io_context ios;

    //create vector to hold the active sockets
    std::vector<asio::ip::tcp::socket> socketVector;

    tcpServer server = tcpServer(ios);

    //findAndConnect(socketVector, ios);
    
    //code below is commandline necessary for attempting to file file name
    //in cmd string placeholder has been added for the file location
    // can also use HELP FC in commandline to get instructions of how /b and etc works
    //example code line for below: fc "c:\Program Files\placeholder_file\filename1.txt" "c:\Program Files\placeholder_file\filename1.txt" \B
    //another example for folder comparison: fc "c:\Program Files\holderfolder\folder1\*" "c:\Program Files\holderfolder\folder2\*" 
    //example above would check if folder2 has all exact files in folder1, from title names to content in files
    string cmd = exec("fc \"insertfilenamehere.txt\" \"insertfilename2here.txt\" /B");
    cout << "the output of command: " << cmd << endl;
    //string required to be found to see if files are the same 
    string check = "FC: no differences encountered";
    if (cmd.find(check) != std::string::npos)
    {
        //files resulted in no differences
        cout << "Files are the same.";
    }
    else
    {
        //files are found to be different
        cout << "Files are not the same.";
    }

    ios.run();
}
