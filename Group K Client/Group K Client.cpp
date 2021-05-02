// Group K Client.cpp : This file contains the 'main' function. Program execution begins and ends there.
//


#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
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

int printMenuGetUserInput() { // currently buggy where userinputstr isn't being used as a string
    std::cout << "1 - Send file to server\n2 - Get file from server\n3 - Exit\nInput: ";
    std::string userinputstr = "";
    std::cin >> userinputstr;
    std::cout << userinputstr;
    while (userinputstr != "1" || userinputstr != "2" || userinputstr != "3") {
        std::cout << "Invalid input. Try again: ";
        std::cin >> userinputstr;
    }
    return std::stoi(userinputstr);
}

FILE* readFileFromUserInputDirectory() {
    std::cout << "Directory of the file: ";
    std::string fileDirectory = "";
    std::cin >> fileDirectory;
    const char* fileDirectoryChar = fileDirectory.c_str();

    FILE* fi;
    errno_t err;

    if ((err = fopen_s(&fi, fileDirectoryChar, "rb")) != 0) {       // try to open the file
        // The file couldn't be opened, fi is set to NULL
        std::cout << "Cannot open given file.\n";
    }
    else {
        // File was opened, fi can be used to read the stream
        fclose(fi);                                                 // close the file
    }
    return fi;
}

void writeHandler(const system::error_code& err, std::size_t bytes_transferred) {
    if (!err) {
        std::cout << "Stop the sending of this file... somehow\n";
    }
    else {
        std::cout << "Something went wrong in handelWrite()! " << err.message() << "\n";
    }
    std::cout << "File sending in progress in write handler...\n";
}

// void writeFileToSocket(FILE* file, const char* buffer, int elementSize, int BUFFER_SIZE, asio::ip::tcp::socket& soc, const system::error_code& err)
// void writeFileToSocket(FILE* file, asio::ip::tcp::socket& soc)
void writeFileToSocket(asio::ip::tcp::socket& soc) {
    #pragma region Get file directory
    std::cout << "Directory of the file: ";
    std::string fileDirectory = "";
    std::cin >> fileDirectory;
    const char* fileDirectoryChar = fileDirectory.c_str(); // GET FILE

    FILE* fi;
    errno_t err;
    #pragma endregion

    int numOfBytes = 0;

    std::vector<char> data(soc.available());                                // Get the amount of data avalible in the socket
    char buffer[100];

    if ((err = fopen_s(&fi, fileDirectoryChar, "rb")) != 0) {               // try to open the file
    // The file couldn't be opened, fi is set to NULL
        std::cout << "Cannot open given file.\n";
    }
    else {
        // File was opened, fi can be used to read the stream
        while (!feof(fi)) {
            if (fgets(buffer, 100, fi) == NULL) {
                std::cout << "File has finished reading.\n";
                break;
            }
            fputs(buffer, stdout);                                          // prints out the contents of the file to console
            soc.async_write_some(boost::asio::buffer(data), writeHandler);  // writes the content to the socket async
        }
        fclose(fi);                                                         // close the file
    }
    // old buffer code: boost::asio::buffer(data)
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

//this function will detect all IP addresses within a subnet, if a vLAN exists, it will only detect IPs in the boundaries of the vLAN
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
            //set start index to the current index
            startIndex = i;
        }

        for (int i = 0; i < ipList.size(); i++)
        {
            std::cout << ipList[i] << std::endl;
        }
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
    //std::string targetRawIP = "239.255.0.1";
    unsigned short targetPort = 3333;


    // I/O context that is used as a basis for communicating I/O commands to the OS
    asio::io_context ios;

    asio::ip::tcp::endpoint clientEndpoint;

    std::vector<std::string> ipList = findIP();


    int BUFFER_SIZE = 256;
    char buffer[256];

    FILE* fi = NULL;

    for (int i = 0; i < ipList.size(); i++)
    {
        try
        {
            // Create an endpoint designating target server application.
            // this will be used by the sockets as an access point for communication using TCP
            std::cout << ipList[i] << " " << ipList[i].size() << std::endl;
            asio::ip::tcp::endpoint clientEndpoint(asio::ip::address::from_string(ipList[i]), targetPort);


            // Create and open a socket.
            // this will allow the socket to be used
            asio::ip::tcp::socket socket(ios, clientEndpoint.protocol());

            // Connect a socket.
            // this will attempt to connect the socket to the server
            socket.connect(clientEndpoint);


            std::cout << "c" << std::endl;

            // THIS IS WHERE JESS STARTS TO BREAK CODE
            fi = readFileFromUserInputDirectory(); //  Get file from directory 
            //writeFileToSocket(socket);
            std::cout << "\nFile written to socket... kinda" << std::endl;
            // Send file to socket
            
            // THIS IS THE END OF WHERE JESS BREAKS CODE

        }
        catch (system::system_error& e) {
            std::cout << "Error occured! Error code = " << e.code()
                << ". Message: " << e.what() << std::endl;;

            //return e.code().value();
        }
    }

    try {
        boost::system::error_code ec;
        std::ostringstream os;
        os << "Message";

        

        /*std::string message_ = os.str();
        while (true)
        {

            socket.async_send_to(
                boost::asio::buffer(message_, 7), clientEndpoint,
                handle);
        }*/
    }
    // Overloads of asio::ip::address::from_string() and 
    // asio::ip::tcp::socket::connect() used here throw
    // exceptions in case of error condition.
    catch (system::system_error& e) {
        std::cout << "Error occured! Error code = " << e.code()
            << ". Message: " << e.what();

        return e.code().value();
    }

    // BROKEN MENU STUFF
    //int menuChoice = printMenuGetUserInput();
    //while (menuChoice != 3) {
    //    if (menuChoice == 1) {
    //        
    //    }
    //    else if (menuChoice == 2) {
    //        std::cout << "Receive file to be implemented!";
    //    }
    //    menuChoice = printMenuGetUserInput();
    //}
    //std::cout << "Exiting application.";

    std::system("pause");
    return 0;
}