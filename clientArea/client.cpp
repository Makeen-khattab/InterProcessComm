/**********************************************************************
 * Author: Makeen Khattab		                                      *
 * Date:   1/9/2024		                                              *
 * Description: Client Server										  *
 *                                                                    *
 *                                                                    *
 **********************************************************************/


/***************************** Refrencing *****************************/
#include <stdio.h>

#include <stdlib.h>

#include <unistd.h>

#include <string>

#include <string.h>

#include <sys/types.h>

#include <sys/socket.h>

#include <netinet/in.h>

#include <netdb.h>

#include <iostream>


/***************************** MACROS *******************************/
constexpr unsigned int SERVER_PORT = 50544;

constexpr unsigned int MAX_BUFFER = 128;

#define INTERPROCESS                        "127.0.0.1"


/*****************************  Main  *******************************/
int main(int argc, char *argv[])

{
/*
Creating a Struct that carries information about the server.
*/    
    struct hostent* server;

/*
Creating a socket specified to Local Communication(Inter-Process Communication)
*/
int sockfd = socket(AF_INET, SOCK_STREAM, 0);

/*
Check if the socket was created successfully
*/
if (sockfd < 0)

{

/*
Informing the User that there has been a problem while
creating the socket
*/
std::cerr << "socket error" << std::endl;
/*
return a flag of one which is stored in errno to indicate for the type of error
*/
return 1;

}


/* Checking the user prefrence to the type of communication*/
if(strcmp(argv[1],"interProcessComm") == 0)
{
server = gethostbyname(INTERPROCESS);

}
else
server=gethostbyname(argv[1]);


/* 
This condition checks the DNS/IP passed by the user for a specific 
Server network to check if it exists or not.
*/
if (server == nullptr)

{

std::cerr << "gethostbyname, no such host" << std::endl;

return 2;
}



// 1. Init any un-init variable.


/*
Struct gathers information about the server socket 
!Note we previously predefined the address of PORTID as a constant NO
*/    
struct sockaddr_in serv_addr;

/*
Filling the Struct with Null for strings and zero for integers
*/
bzero((char *) &serv_addr, sizeof(serv_addr));

/*
Specifying the Network IP version
For instance:IPV4.
*/
serv_addr.sin_family = AF_INET;

// 2. copy struct to struct.
bcopy((char *)server->h_addr,
(char *)&serv_addr.sin_addr.s_addr,
server->h_length);
/*
Specifying the PORTID of the Server Network
And pass it to the server info struct
*/ 
serv_addr.sin_port = htons(SERVER_PORT);

/*
*
*
After gathering all info about the server we will try
to connect over the server socket by a socket to socket communication
*
*
*/

/*
Checking the connection status
*/
if ((connect(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)))< 0)
{

std::cerr << "connect error" << std::endl;

return 3;

}

/*
*
*
Creating a string to receive the messages
from the server
*
*
*
*/
std::cout<<"----------------------------------"<< std::endl;
std::cout<<"--- Start of the Communication ---"<< std::endl;
std::cout<<"----------------------------------"<< std::endl<<std::endl;
while(1)
{
std::string readFromServer (MAX_BUFFER, 0);

/*
Reading from the server
And checking the status
*/
std::cout<<"----------------------------------"<< std::endl<<std::endl;
std::cout <<"Server: "<<std::flush;
if (read(sockfd, &readFromServer[0], MAX_BUFFER-1) < 0)

{

std::cerr << "read from socket failed" << std::endl;

return 5;

}
/*
Displaying the server message
*/
std::cout<<readFromServer << std::endl;

/* 
*
*
*
Ending the communication between the Server
and the client
*
*
*
*/
if(readFromServer.compare("terminateComm ")==0)
{
    std::cout<<"----------------------------------"<< std::endl;
    std::cout<<"---  End of the Communication  ---"<< std::endl;
    std::cout<<"----------------------------------"<< std::endl;
close(sockfd);

return 0;

}

/*
Creating a client string to be sent to the server
for the communication
*/
std::string writeBuffer (MAX_BUFFER, 0);

/*
Taking a string as an input from the user
*/
std::cout << "Client message to be sent: ";
getline(std::cin, writeBuffer);
std::cout<<std::endl;
std::cout<<"----------------------------------"<< std::endl<<std::endl;
/*
Communicating with the server by a write operation.
*/
if (write(sockfd, writeBuffer.c_str(), strlen(writeBuffer.c_str())) < 0)

{

std::cerr << "write to socket" << std::endl;

return 4;

}

}

return 0;

}
