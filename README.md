# Simple C Chat Server and Client

A lightweight, terminal-based chat application written in C. It uses TCP sockets and the `select()` system call to handle multiple clients concurrently without multi-threading.

## Features

* **Multi-client support:** The server handles up to 10 connected clients simultaneously using `select()`.
* **Message Broadcasting:** Messages sent by any client are prefixed with their chosen name and broadcasted to all connected clients.
* **Chat History:** The server maintains a history of the last 100 messages. When a new user connects, they automatically receive the chat history so they can catch up on the conversation.
* **Non-blocking I/O:** Both the client and server use `select()` to monitor standard input and network sockets at the same time.

## Prerequisites

* A Linux/Unix-based operating system (or WSL on Windows) to support POSIX sockets (`<sys/socket.h>`, `<sys/select.h>`, etc.).
* `gcc` or any standard C compiler.

## Compilation

Open your terminal and compile the two source files separately:

```bash
# Compile the server
gcc server.c -o server

# Compile the client
gcc client.c -o client
```

## Usage

1. **Start the server:**
   Run the server first. It will listen for incoming connections on port `8080`.
   ```bash
   ./server
   ```

2. **Start the client(s):**
   Open a new terminal window (or multiple windows for multiple clients) and run:
   ```bash
   ./client
   ```

3. **Chatting:**
   * Upon connecting, the client will prompt you to enter your name.
   * Type your name and press Enter.
   * You will receive any previous chat history from the server.
   * Type your messages and press Enter to broadcast them to everyone in the chat room.

## File Structure

* `server.c`: Contains the server logic (socket binding, listening, accepting new clients, maintaining user structs, managing message history, and broadcasting).
* `client.c`: Contains the client logic (connecting to `127.0.0.1:8080`, taking user input, and displaying received messages).
