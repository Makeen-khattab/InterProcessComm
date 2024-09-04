# Intro

This recipe will show you how to connect two programs by using a connection-oriented mechanism. 
This recipe will use TCP/IP, which is the `de facto` standard on the internet. So far, we've learned that TCP/IP is a reliable form of communication, and its connection is made in three phases. Although the language used will be C++, the communication part will be written using the Linux system calls, as it is not supported by the C++ standard library. 
As a result we will include libraries from the `glibc` which will Isolate of implementing **Direct System Call**.

# Client Server

## Referencing

We will show now the Libraries that is needed to be included from the `glibc` to isolate us from the `syscalls` to utilize the HW.

Also we have defined two constant expr:
1) **Represent the PORT which the client will consider as the Destination for the service server.**
   
2) **Buffer size represent the maximum number of requests that can be handled for the service server before losing a request.** 

```CPP
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <iostream>

constexpr unsigned int SERVER_PORT = 50544;
constexpr unsigned int MAX_BUFFER = 128;

```

## Socket creation

We will now use the manual for the `glibc` in order to know what are the `syscalls` Related with the socket initialization.

**Header File** :

```CPP
#include <sys/socket.h>
```

**Function Prototype**:
```CPP
int socket(int domain, int type, int protocol);
```

**Function Purpose**:

`socket()` : 

Creates an ==endpoint== for communication and returns a file descriptor that refers to that endpoint. 

Endpoint means the combination between the IP address of the Host in our case is the client and the PORTID created for that socket

It can be mathematically represented as:

`Endpoint` = `IP` | `PORTID`

The File descriptor returned by a successful call will be the Lowest-numbered file descriptor not currently open for the process.

Each time your program opens a ==new file, socket, or another resource==, the operating system assigns it a `file descriptor`. The file descriptors are typically ==small integers== (e.g., 0, 1, 2, 3, …). 

When the `socket()` function successfully creates a new socket, the operating system looks for the smallest integer `(file descriptor)` that ==isn't already in use by your process== and assigns it to the new socket.

**Function Arguments**:

`domain`: 
specifies a communication domain, this
selects the protocol family which will be used for communication.
![image](https://github.com/user-attachments/assets/6a9701cd-5ed0-4f23-949c-5587728392b8)


`type`: which specifies the communication semantics

What is meant by communication semantics is:

1) **Delivery Guarantees**:
   
   - **Reliable vs. Unreliable**: Whether the communication system guarantees that messages will be delivered (reliable) or if they might be lost (unreliable). 
 
   - **Ordered vs. Unordered**: Whether messages will be delivered in the order they were sent or if they might arrive out of sequence.

2) **Message Interpretation**:
   
   - **Synchronous vs. Asynchronous**: Whether the sender waits for a response after sending a message (synchronous) or continues its operation without waiting (asynchronous).
   - **Blocking vs. Non-blocking**:Whether the communication operation waits until it completes (blocking) or returns immediately to allow other operations to continue (non-blocking).

3) **Error Handling**
   - **Timeouts**: What happens if no response is received within a certain time frame.
   - **Retries**: Whether and how the system attempts to resend messages after a failure.

We have different types provided for the `socket()` you can preview from the man page according to your Logic and Implementation.

`protocol`: 
A protocol is essentially an agreement on how communication should occur. For example, when two computers communicate over the internet, they must agree on how to format and transmit the data, so they both understand the information being exchanged.

For instance:
1) **TCP**
2) **UDP**
When you create a socket using the `socket()` function, you may need to specify which protocol you want to use with that socket. For example, you might choose TCP or UDP, depending on whether you need reliable communication or not.

In many cases, the combination of the socket type and the protocol family automatically determines the protocol, so you can set the protocol parameter to `0`, and the system will choose the appropriate protocol for you.

**Function Return Value**:
 On success, a file descriptor for the new socket is returned.  On error, -1 is returned, and `errno` is set to indicate the error.


**Implementation**:

```CPP
int main(int argc, char *argv[])
{ 

int sockfd = socket(AF_INET, SOCK_STREAM, 0); 

/*Checking the socket is successfully created or not */
if (sockfd < 0) 
{ 
std::cerr << "socket error" << std::endl; 
 
 return 1; 
}
  
```


## Getting IP Address of the Host 

**Header File** :

```CPP
#include <netdb.h>
```

**Function Prototype**:
```CPP
struct hostent *gethostbyname(const char*name);
```

**Function Purpose**:

`gethostbyname()` : 

The primary purpose of `gethostbyname()` is to perform DNS (Domain Name System) resolution, which involves converting a ==hostname== into an ==IP address==. 


**Function Argument:**

Takes the DNS name from the program as an input and store in a struct the IP address of that host and the Length.

 **RETURN VALUE**:

The `gethostbyname()` functions return the
 `hostent structure` or a `null pointer` if an error occurs

The `hostent structure`:

```CPP
struct hostent 
{
char  *h_name;     /* official name of host*/
char **h_aliases;         /* alias list */
int    h_addrtype;        /* host address type */
int    h_length;          /* length of address */
char **h_addr_list;       /* list of addresses *   }

```

**Implementation:**

```CPP
struct hostent* server = gethostbyname(argv[1]); if (server == nullptr) 
{
std::cerr << "gethostbyname, no such host" << std::endl; 
 return 2; 
}

```

## Connecting to the Server


`void bzero(void s[.n], size_t _n_);`:

The `bzero()` function erases the data in the `n` bytes of the memory starting at the location pointed to by `s`, by writing zeros (bytes containing '\0') to that area.

---

`void bcopy(const void src[.n],void dest[.n], size_t n);`:

 The `bcopy()` function copies `n` bytes from `src` to `dest`The result is correct, even when both areas overlap.

---


```CPP

struct sockaddr_in 
{
sa_family_t sin_family;  /* AF_INET*/
in_port_t   sin_port;    /* Port number */
struct in_addr sin_addr; /* IPv4 address */

};

```
**AF**: Stands for "Address Family."

---

```CPP
struct sockaddr_in serv_addr;

bzero((char *) &serv_addr, sizeof(serv_addr)); 

serv_addr.sin_family = AF_INET; 

bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length); 

serv_addr.sin_port = htons(SERVER_PORT); 

if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof (serv_addr)) < 0)
{

std::cerr << "connect error" << std::endl;

return 3;

}

```


## Response From Server


**Header File** :

```CPP
 #include <unistd.h>
```

**Prototype**:

```CPP
ssize_t read(int fd, void buf[.count],size_t count);
```

**Purpose**:
`read()` attempts to read up to `count`bytes from file descriptor `fd` into the buffer starting at `buf`.

On files that support seeking, the read operation commences at the file offset, and the file offset is incremented by the number of bytes read.  If the file offset is at or past the end of file, no bytes are read, and `read()` returns zero.

If `count` is zero, `read()` may detect the errors described below.
In the absence of any errors, or if **read**() does not check for errors, a **read**() with a `count` of 0 returns zero and has no other effects.

On error, `-1` is returned, and `errno` is set to indicate the error. In this case, it is left unspecified whether the file position (if any) changes.

```CPP
std::string readBuffer (MAX_BUFFER, 0); 
if (read(sockfd, &readBuffer[0], MAX_BUFFER-1)< 0) 
{ std::cerr << "read from socket failed" << std::endl;
 return 5; 
 } 
 
 std::cout << readBuffer << std::endl;


```

## Sending Data to the Server

**Header File** :

```CPP
 #include <unistd.h>
```


```CPP
std::string writeBuffer (MAX_BUFFER, 0); std::cout << "What message for the server? : "; getline(std::cin, writeBuffer); 
if (write(sockfd, writeBuffer.c_str(), strlen(write Buffer.c_str())) < 0) 
{ 
std::cerr << "write to socket" << std::endl; 

return 4; 
}


```

## Closing the Socket

```CPP
close(sockfd);
return 0;
```



# Service Server

We will show now the Libraries that is needed to be included from the `glibc` to isolate us from the `syscalls` to utilize the HW.

Also we have defined two constant expr:
1) **Represent the PORT which the client will consider as the Destination for the service server.**
   
2) **Buffer size represent the maximum number of requests that can be handled for the service server before losing a request.** 

```CPP
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <iostream>

constexpr unsigned int SERVER_PORT = 50544;
constexpr unsigned int MAX_BUFFER = 128;
constexpr unsigned int MSG_REPLY_LENGTH =18;

```


## Creating a Socket

The `setsockopt()` function shall set the option specified by the
   `option_name` argument, at the protocol level specified by the
   `level` argument, to the value pointed to by the `option_value`
   argument for the socket associated with the file descriptor
   specified by the `socket` argument.


```CPP
int main(int argc, char *argv[])
{ 
int sockfd = socket(AF_INET, SOCK_STREAM, 0); 

if (sockfd < 0)
{ 
std::cerr << "open socket error" << std::endl

return 1; 
} int optval = 1; 
setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval , sizeof(int))

```


## Binding Socket

Binding a socket with a certain `IP address` and `PORTID`.

The main difference between the Binding in the `Client server` & `Service Server`:

1) Binding in the `Client server` is done automatically on creating the Socket with it's IP and a random PORTID.
   
2) Binding in the `Service server` is modified after creating the socket to select a specific PORTID that any `Client server` can connect over as a result it can't be random as in the case of `Client server` 


```CPP
struct sockaddr_in serv_addr, cli_addr; 

bzero((char *) &serv_addr, sizeof(serv_addr));

serv_addr.sin_family = AF_INET; 

serv_addr.sin_addr.s_addr = INADDR_ANY; 

serv_addr.sin_port = htons(SERVER_PORT); 

if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof (serv_addr)) < 0) 
{ 
std::cerr << "bind error" << std::endl;

return 2;

} 
```

## Listen


```CPP

listen(sockfd, 5);
socklen_t clilen = sizeof(cli_addr); 
int newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen); 
if (newsockfd < 0) 
{ 
std::cerr << "accept error" << std::endl; 
return 3; 
}



```

### 1. `listen(sockfd, 5);`

- **Purpose**: This function call tells the operating system that the server is ready to accept incoming connection requests on the socket `sockfd`.
- **Parameters**:
  - `sockfd`: The file descriptor for the server's listening socket, which was created and bound to a port earlier in the code.
  - `5`: This is  specifies the maximum number of connections that can be queued for acceptance. If more than 5 connection requests come in before the server accepts any of them, additional requests may be rejected or ignored by the OS.

### 2. `socklen_t clilen = sizeof(cli_addr);`

- **Purpose**: This line sets the size of the `cli_addr` structure, which will hold the client's address information once a connection is accepted.
- **`socklen_t clilen`**: This is a variable that holds the size of the `cli_addr` structure, typically a `sockaddr_in` structure for IPv4.
- **`sizeof(cli_addr)`**: This calculates the size of the `cli_addr` structure.

### 3. `int newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);`

- **Purpose**: The `accept()` function is called to accept an incoming connection request from a client. When a client attempts to connect to the server, `accept()` extracts the first connection request from the queue of pending connections (created by `listen()`) and creates a new socket for this connection.
    
- **Parameters**:
    
    - `sockfd`: The file descriptor of the listening socket.
    - `(struct sockaddr *) &cli_addr`: A pointer to a `sockaddr` structure (or `sockaddr_in` for IPv4) that will be filled with the client's address information (IP address and port) after the connection is accepted.
    - `&clilen`: A pointer to a variable that initially contains the size of the `cli_addr` structure. After `accept()` returns, this variable will be updated with the actual size of the address returned.
- **Return Value**:
    
    - If successful, `accept()` returns a new file descriptor (`newsockfd`) that refers to the socket connected to the client. This socket is used for communication with the client.
    - If `accept()` fails, it returns `-1`, and an error message is printed using `std::cerr`.

### 4. `if (newsockfd < 0) { std::cerr << "accept error" << std::endl; return 3; }`

- **Purpose**: This checks if `accept()` returned an error (indicated by a negative value). If it did, the code prints an error message and exits the program with a return code of `3`.
- **`std::cerr << "accept error" << std::endl;`**: This prints an error message to the standard error stream, indicating that something went wrong while trying to accept a connection.
- **`return 3;`**: The program exits with a return code of `3`, signaling that an error occurred. This return code can be useful if you are debugging or running the server in a script where you want to handle errors.


## Ack for connection

```CPP
std::cout << "server: got connection from = " << inet_ntoa(cli_addr.sin_addr) << " and port = " << ntohs(cli_addr.sin_port) << std::endl;

write(incomingSock, "You are connected!", MSG_REPLY_LENGTH);


```


## Writing 

```CPP
std::string buffer (MAX_BUFFER, 0);
if (read(incomingSock, &buffer[0], MAX_BUFFER-1) < 0) 
{ 
std::cerr << "read from socket error" << std::endl; 
return 4; 
} 
std::cout << "Got the message:" << buffer << std::endl;

```

## Closing Socket

```CPP
close(incomingSock); 
close(sockfd); 
return 0;

```
