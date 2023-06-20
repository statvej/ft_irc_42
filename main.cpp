#include "irc.hpp"
#include "Server.hpp"

Server* Server::serverPointerForSignal = nullptr;

int main(int ac, char **av) {
	if(ac == 4 || ac == 3){
		std::string password(av[2]);
		int port = atoi(av[1]);
		std::string key("");
		if(ac == 4)
			key = av[3];
		Server irc_server(port, password, key);
		Server::serverPointerForSignal = &irc_server;
		irc_server.loop();
	}
	else
		std::cerr << "Wrong number of arguments" << std::endl;
}
