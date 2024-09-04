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

#include <string.h>

#include <string>

#include <string>

#include <unistd.h>

#include <sys/types.h>

#include <sys/socket.h>

#include <netinet/in.h>

#include <iostream>

#include <arpa/inet.h>

#include <thread>


/***************************** MACROS *****************************/
constexpr unsigned int SERVER_PORT = 50544;

constexpr unsigned int MAX_BUFFER = 128;

constexpr unsigned int MSG_REPLY_LENGTH = 18;


int main(int argc, char *argv[])
{

int incomingSock;

unsigned char oneTimeOnly=1;

/*
Creating a socket Identified for Inter-Process Communication
Storing the status of creation into the variable sockfd
*/
int sockfd = socket(AF_INET, SOCK_STREAM, 0);

/*
*
Check if the socket was created successfully
*/
if (sockfd < 0)
{
/*
Informing the User that there has been a problem while
creating the socket
*/
std::cerr << "open socket error" << std::endl;
return 1;
/*
return a flag of one which is stored in errno to indicate for the type of error
*/
}

/*
Configuring the server socket settings
*/
int optval = 1;

setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval , sizeof(int));

/*
Struct gathers information about the server socket 
!Note we previously predefined the address of PORTID as a constant NO
*/   
struct sockaddr_in serv_addr, cli_addr;

bzero((char *) &serv_addr, sizeof(serv_addr));

serv_addr.sin_family = AF_INET;

serv_addr.sin_addr.s_addr = INADDR_ANY;

serv_addr.sin_port = htons(SERVER_PORT);

/*
Binding the IP address with the PORTID to form the Socket address

1) Binding in the client server was done Automatically on creation
of the socket with it's IP and a random PORTID.

2) Binding in the service server is modified after creation to select
a specific PORTID that the Client server was informed of to be able to 
connect successfully.
*/

if ((bind(sockfd, (struct sockaddr *) &serv_addr, sizeof (serv_addr))) < 0)

{
    /*
    Informing the Service that the Binding process failed 
    For instance due to PORTID Unavailabiliity
    */

std::cerr << "bind error" << std::endl;

/*
return a flag of one which is stored in errno to indicate for the type of error
*/
return 2;

}

/*
The server in this part will remain waiting for a connection 
request from any client, It can handle 5 clients at the same time without
rejecting any of them but it can process one at a time.
*/
listen(sockfd, 5);

socklen_t clilen = sizeof(cli_addr);

/*
Reading from the Client server the info related
by the service like: 
1- Socket address to be connected over.
2- IP version
3- Protocol TCP/UDP
*/

incomingSock = accept(sockfd, (struct sockaddr *) &cli_addr,&clilen);


std::cout << "server: got connection from = "<< inet_ntoa(cli_addr.sin_addr)

<< " and port = " << ntohs(cli_addr.sin_port)

<< std::endl;

std::string messageByServer;

messageByServer.assign("Connected!");

std::cout<<"----------------------------------"<< std::endl;
std::cout<<"--- Start of the Communication ---"<< std::endl;
std::cout<<"----------------------------------"<< std::endl;

unsigned char firstEntery=0;


while(1)
{
/*
If connected Successfully
The service will Inform the User
*/
while(!firstEntery++)
{
    std::cout<<"----------------------------------"<< std::endl<<std::endl;
    std::cout << "server: "<<messageByServer<<std::endl;
}

write(incomingSock, messageByServer.c_str() , messageByServer.length());

if(messageByServer.compare("terminateComm")==0)
{
    std::cout<<"----------------------------------"<< std::endl;
    std::cout<<"---  End of the Communication  ---"<< std::endl;
    std::cout<<"----------------------------------"<< std::endl;
close(sockfd);
close(incomingSock);
return 0;

}

/*
Declaring a String for storing Client message
*/
std::string messageFromClient (MAX_BUFFER, 0);

/*
Checking the status of the message sent  by client
*/
std::cout << "Client-mess: "<<std::flush ;
if (read(incomingSock, &messageFromClient[0], MAX_BUFFER-1) < 0)
{
std::cerr << "read from socket error" << std::endl;

}

/*
Displaying the Client message
*/
std::cout<< messageFromClient << std::endl;

/*
Taking another input message by Server to be sent
*/
std::cout<<"----------------------------------"<< std::endl<<std::endl;
std::cout << "server: ";
getline(std::cin,messageByServer);
std::cout<<std::endl;

}

return 0;

}