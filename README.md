# Multithreaded-Chatroom

## Project-Description

This a Chatroom built in C++ using the concept of Multithreading and Synchronization, Multiple Users can Chat using this application.

## Dependencies Required
The Application only needs a linux environment for testing and the lpthreads should be working so make sure that you have libboost installed in your environment.
For that run the following command:
```
$ sudo apt-get install libboost-all-dev
```

## How to Run?
1. Clone this Repository
2. Go to the directory where you have cloned the repository and run the following commands:
```
$ g++ server.cpp -lpthread -o server
$ g++ client.cpp -lpthread -o client
```
3. Now Run the server using the following command:
```
$ ./server
```
4. You can run client using the following command:
```
$ ./client
```
5. For having Multiple clients you can perform Step-4 multiple times.
## Internal Working of the Project
The implementation portion is divided into 2 parts:
1. Server Side
2. Client Side

### Server Side Implementation Details
Firstly, I have created a socket using `socket` function, and then Binded it (i.e assigned an IP and PORT to the socket which is necessary for connection) using the
`bind` function, at each step I am taking care of the errors and I am displaying an error message if any of the error arises, after that I have marked the socket passive using the `listen` function, marking it passive enables the socket to accept connections. Then I have accepted connections from the clients.
(Here the request for conncetion is analogous to a user joining the chatroom) 
I have implemented the synchronisation (Use of Mutex) to handle differenct clients which in case are the users of the chat room, I have used `lock_guard` method as
the mutex wrapper. There are also various Utility functions for various purposes (like assigning colors, handling clients).
### Implementation Details of Some Utility Functions 
`send_message` function has two overloaded functions, one of them is used to broadcast a message to all the users except the sender which is taken care of by
using the `sender ID` which is a unique ID assigned to each user, the other function of the same name is used to broadcast a number.
`set_name` function is used to assign name to a user with ID given as the parameter. `shared_print` function is used to synchronize cout statments in case there is 
a scope for deadlock among users while messaging.
`end_connection` function is called when a user leaves the chat, this function closes the connection for that particular user. `handle_client` is the main function
which is used to handle a particular client, it displays the welcome message, assigns color using the `color` function and takes care of all the functionalities 
and features available for a current user. There is and option to leave the chatroom by typing the message `$exit`.

### Client Side Implementation Details
This side has the send and receive message functionality (i.e `send_message` and `recv_message` respectively), which can be used after a user has established connection with the server, any message sent and received will be displayed by the server.I have also implemented a handler for `CTRL+C` which can be used as an 
alternative to leave the chatroom.
There are 2 threads `t1` and `t2` each for sending message and recceiving so that each user can send and receive message indpendent of other users receiving or sending. This is where threading comes in to play.

## Learnings from the Project
Following are the things that I have learned during the course of the Project:
1. I am now able to clearly what threading is and what is it's importance in the large scale projects.
2. I have learned about the concept of Mutual-Exclusion (Mutex) in practical, I previously only had the theoretical knowledge of it.
3. I got to learn about many threading related functions that C/C++ has to offer , which I previously had no idea that they even exist.

## Video Demo


https://user-images.githubusercontent.com/81903271/174816440-482c435b-c2b8-42b3-b565-10f6bc0982a0.mp4


## References and Resources Used
1. [Socket Programming in C++](https://www.geeksforgeeks.org/socket-programming-cc/)
2. [Multithreading in C++](https://www.geeksforgeeks.org/multithreading-in-cpp/)
3. [Multithreading in C++](https://www.tutorialspoint.com/cplusplus/cpp_multithreading.html)
4. [Internals of Threads in C++](https://cplusplus.com/reference/thread/thread/)
5. [Building a Server](https://ncona.com/2019/04/building-a-simple-server-with-cpp/)
