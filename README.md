# ft_irc

Welcome to ft_irc, an IRC (Internet Relay Chat) server project implemented in C++. This project is part of the core curriculum at ECOLE42.

## Project Description

The ft_irc project aims to create an IRC server that can handle multiple clients simultaneously. It provides a platform for users to connect, communicate, and collaborate in real-time. The server supports various commands for managing channels and incorporates a built-in chat bot for enhanced functionality.

## Features

- **Multi-client Support:** The server is designed to handle multiple clients concurrently, allowing users to connect from different machines and communicate with each other.

- **Channel Management:** Users can create, join, and leave channels. They can also list all available channels and retrieve information about a specific channel.

- **Chat Bot:** The built-in chat bot enhances the user experience by providing additional functionality. The bot can respond to specific commands or interact with users in predefined ways, by getting answers from ChatGPT API.

## Usage

To use ft_irc, follow these steps:

1. Clone the ft_irc repository to your local machine.
   '''shell
   git clone https://github.com/statvej/ft_irc_42.git
   ''';

2. Navigate to the project directory.
   '''shell
   cd ft_irc
   ''';

3. Build the project using your preferred C++ compiler.
   '''shell
   make
   ''';

4. Run the server.
   '''shell
   ./ft_irc
   ''';

5. Connect to the server using an IRC client application. Use the server address and port to establish a connection.

6. Start interacting with the server by joining channels, sending messages, and utilizing the available commands.
