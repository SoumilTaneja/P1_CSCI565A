Name - Soumil Taneja
CWID - 10924723

## Project Overview

This project is a client/server message routing system implemented using both TCP and UDP sockets. The system is composed of two main components:

1. **Server**: Receives, stores, and forwards messages to clients. It operates as a continuously running application that maintains a shared message queue.
2. **Client**: Sends and receives messages from the server. Clients can be started in either "send" mode or "receive" mode.

## File Structure

- `p1client.c`: The client application source code.
- `p1server.c`: The server application source code.
- `message1.txt`: A sample text file containing the message "This is Message 1".
- `message2.txt`: A sample text file containing the message "This is Message 2".
- `Makefile`: A Makefile for compiling the client and server applications.
- `README.md`: This documentation file.

## Compiling the Project

To compile the project, use the provided Makefile. This will compile both the client and server applications.

## How to Test This Project

1. **Compile the project** using `make`.

2. **Start the server** using `./p1server <port> <TCP|UDP>`.

   Example: `./p1server 55555 TCP`

3. **Open a new terminal** for the client.

4. **Send a message** to the server using `./p1client <IP> <port> <TCP|UDP> send <file>`.

   Example: `./p1client 127.0.0.1 55555 TCP send message1.txt`

5. **Receive a message** from the server using `./p1client <IP> <port> <TCP|UDP> receive`.

   Example: `./p1client 127.0.0.1 55555 TCP receive`

6. **Send another message** to the server using the same send command.

7. **Receive the next message** from the server using the receive command.

8. **Check for an empty queue** by running the receive command after all messages have been received.

9. **Test with multiple clients** by opening additional terminals and repeating the send and receive steps.

10. **Shut down the server** by pressing `Ctrl+C` in the server terminal.

Project 1 completed!
