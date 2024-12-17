# Chat Server and Client using Qt

This project consists of a server and client implementation using the Qt framework. The server handles multiple client connections, supports username assignment, message broadcasting, and private messaging. The client provides a graphical interface for login, chat, and message handling, with functionality for both private and group messaging.

## Features

### Server:
- Accepts incoming client connections.
- Automatically assigns random usernames to clients if the provided one is already taken.
- Allows clients to send messages to all other clients (group chat).
- Supports private messaging between clients.
- Keeps an updated list of connected users, which is broadcast to all clients.
- Handles client disconnections gracefully.

### Client:
- Provides a GUI for users to input their username and connect to the server.
- Displays the list of connected users.
- Allows users to change their username (if not already taken).
- Supports sending messages to all users or to a specific user (private messaging).
- Displays chat messages in a text box.

## Requirements

- Qt 5.15 or later
- C++11 or later
- A TCP/IP network connection

## Building

- Navigate to the desired subproject directory (e.g., Client or Server).
- Run `build.bat` to compile the project.

## Running

- Locate the `build_ninja` folder in each subproject.
- Run the respective executable files (`Client.exe`, `Server.exe`).

## Usage

1. **Start the Server**:
   - Compile and run the server code.
   - The server will listen on port 1234 by default.

2. **Start the Client**:
   - Compile and run the client code.
   - Enter the server IP address and username to connect.
   - Once connected, the client will show the list of users and allow sending messages.

3. **Change Username**:
   - Clients can change their username using the "Change" button in the chat screen.
   - The server will notify all clients of the name change.

4. **Send Messages**:
   - Messages can be sent to all users or to a specific user by selecting a user from the list and sending a message.
