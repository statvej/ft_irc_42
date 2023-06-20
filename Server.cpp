#include "Server.hpp"

Server::Server(int port, std::string password, std::string key)
{
	theadress.sin_family = AF_INET;
	theadress.sin_port = htons(port);
	theadress.sin_addr.s_addr = 0;
	_password = password;
	_serverName = HOST_NAME;
	if ((_sockFD = socket(AF_INET, SOCK_STREAM, 0)) == -1) //domain, type, protocol
		std::cerr << "Couldnt create a socket\n";
	int reuse = 1;
	setsockopt(_sockFD, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
	if (bind(_sockFD, (struct sockaddr*)&theadress, sizeof(theadress)) == -1)
	{
		std::cerr << "Couldnt bind a socket\n";
		exit(0);
	}
	if (listen(_sockFD, 50) == -1)
		std::cerr << "Couldnt start listening\n";
	FD_SET(_sockFD, &_readFDs);
	FD_SET(_sockFD, &_writeFDs);
	ChatBot bot;
	clientsRegistered[BOT_NAME] = new ChatBot(key);
	std::cout << "\n\n\napi kry is" << clientsRegistered[BOT_NAME]->getApiKey() << "\n\n" << std::endl;
	signal(SIGINT, &Server::signalHandler);
}

Server::~Server()
{
	for (std::map<std::string, Channel*>::iterator i = _channels.begin(); i != _channels.end(); ++i) {
		delete i->second;
	}
}

void Server::disconectClient(Client& cl) {
	close(cl.getSockFD());
	if (FD_ISSET(cl.getSockFD(), &_readFDs))
		FD_CLR(cl.getSockFD(), &_readFDs);
	if (FD_ISSET(cl.getSockFD(), &_writeFDs))
		FD_CLR(cl.getSockFD(), &_writeFDs);
	if (clientIsConnected(cl))
	{
		#ifdef DEBUG
		std::cout << "Disconected client with fd " << cl.getSockFD() << std::endl;
		#endif // DEBUG
		if (clientIsRegistered(cl))
			clientsRegistered.erase(cl.getNickname());
	}
}

bool Server::clientIsConnected(Client& cl) {
	return (clients_connected.find(cl.getSockFD()) != clients_connected.end());
}

bool Server::clientIsRegistered(Client& cl) {
	return (clientsRegistered.find(cl.getNickname()) != clientsRegistered.end());
}

bool Server::clientIsRegistered(std::string str) {
	return (clientsRegistered.find(str) != clientsRegistered.end());
}

void Server::connectClient(int fd) {
	FD_SET(fd, &_readFDs);
	Client to_insert(fd);
	to_insert.setUsermode(CONNECTED);
	#ifdef DEBUG
	write(fd, "you got connected\n", 19);
	std::cout << "Client fith fd " << fd << " just connected\n";
	#endif // DEBUG
	clients_connected[fd] = to_insert;
}

void Server::sendMessage(Client* cl, std::string msg) {
	if(cl && FD_ISSET(cl->getSockFD(), &_writeFDs)) {
		send(cl->getSockFD(), msg.c_str(), msg.length(), 0);
	}
}

void Server::sendMessageToAll(std::string msg) {
	std::map<std::string, Client*>::iterator end_iter = clientsRegistered.end();
	for (std::map<std::string, Client*>::iterator i = clientsRegistered.begin(); i != end_iter; i++)
		sendMessage(i->second, msg);
}


void Server::processMessage(char* rawMessage, Client* client) {
	if (rawMessage != NULL)
		client->addToBuffer(std::string(rawMessage));
	#ifdef DEBUG
	std::cout << client->getMessageBuffer();
	#endif // DEBUG
	if (client->getMessageBuffer().find("\r\n") == std::string::npos)
		return;
	Message message((char*)client->getMessageBuffer().c_str());
	#ifdef DEBUG
		std::cout << message << "was sent to a client " << client->getMessageBuffer().find("\r\n") << std::endl;
	#endif // DEBUG
	if (message.getCommand() == "PASS")
		PASS(client, message);
	if (client->getUsermode() >= USER_MODE::PASSED) {
		if (message.getCommand() == "NICK")
			NICK(client, message);
		if (message.getCommand() == "USER")
			USER(client, message);
		if (message.getCommand() == "PRIVMSG")
			PRIVMSG(client, message);
		if (message.getCommand() == "QUIT")
			QUIT(client, message);
		if (message.getCommand() == "JOIN")
			JOIN(client, message);
		if (message.getCommand() == "KICK")
			KICK(client, message);
		if (message.getCommand() == "INVITE")
			INVITE(client, message);
		if (message.getCommand() == "LIST")
			LIST(client, message);
		if (message.getCommand() == "NAMES")
			NAMES(client, message);
		if (message.getCommand() == "TOPIC")
			TOPIC(client, message);
		if (message.getCommand() == "MODE")
			MODE(client, message);
		if (message.getCommand() == "PART")
			PART(client, message);
		if (message.getCommand() == "PING")
			PING(client, message);
	}
	client->flushBuffer();
	#ifdef DEBUG
		std::cout << "buffer after flush " << client->getMessageBuffer() << "buffer len is " << client->getMessageBuffer().length() << std::endl;
	#endif // DEBUG
	processMessage(NULL, client);
}


void Server::loop() {
	char messageBuffer[MESSAGE_BUFFER_SIZE] = { 0 };
	socklen_t name;
	fd_set readFDs;
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	int highestSocket;
	std::cout << "Starting a loop\n";
	while (1) {
		if (clients_connected.size() > 0)
			highestSocket = clients_connected.rbegin()->first;
		else
			highestSocket = _sockFD;
		_writeFDs = readFDs = _readFDs;
		//waits and puts activated sockets into readFDs 
		if (select(highestSocket + 1, &readFDs, &_writeFDs, NULL, &tv)) {
			//if activated socket is servers own socket we got a new client
			if (FD_ISSET(_sockFD, &readFDs)) {
				//accepting new clients
				int new_socket = accept(_sockFD, (struct sockaddr*)&theadress, &name);
				if (fcntl(new_socket, F_SETFL, O_NONBLOCK) == -1) {
					std::cerr << "Error with changing FD to nonblocking\n";
				}
				this->connectClient(new_socket);
			}
			//we need cause when we do manipulations to map end might change
			std::map<int, Client>::iterator end_iter = clients_connected.end();
			//going through all the clients and checking if any of their sockets got active 
			for (std::map<int, Client>::iterator i = clients_connected.begin(); i != end_iter; i++) {
				//if client fd is active we get a message from them
				if (FD_ISSET(i->first, &readFDs)) {
					FD_CLR(i->first, &readFDs);
					if (recv(i->first, messageBuffer, MESSAGE_BUFFER_SIZE, 0) <= 0)
						disconectClient(i->second);
					else
					{
						processMessage(messageBuffer, &i->second);
						bzero(messageBuffer, MESSAGE_BUFFER_SIZE);
					}
					if (clients_connected.empty())
						break;
					i = clients_connected.begin();
					end_iter = clients_connected.end();
				}
			}
		}
	}
}

void Server::checkAndRegister(Client* cl) {
	if (!clientIsRegistered(*cl)
		&& !cl->getNickname().empty()
		&& !cl->getUsername().empty()
		&& !cl->getRealname().empty()) {
		cl->setUsermode(REGISTERED);
		cl->setPrefix(cl->getNickname(), cl->getUsername(), _serverName);
		clientsRegistered.insert(std::make_pair(cl->getNickname(), cl));
		sendMessage(cl, RPL_WELCOME(cl->getNickname(), cl->getUsername(), cl->getHost()));
		sendMessage(cl, std::string(RPL_MOTDSTART(cl->getNickname())));
		sendMessage(cl, RPL_MOTD(cl->getNickname()));
		sendMessage(cl, RPL_MOTDEND(cl->getNickname()));
		#ifdef DEBUG
			std::cout << "client " << *cl << " got registered\n";
		#endif // DEBUG
	}
}

const std::map<int, Client>& Server::getClientsConnected() const {
	return clients_connected;
}

void Server::setClientsConnected(const std::map<int, Client>& clientsConnected) {
	clients_connected = clientsConnected;
}

const fd_set& Server::getReadFDs() const {
	return _readFDs;
}

void Server::setReadFDs(const fd_set& readFDs) {
	_readFDs = readFDs;
}

const t_sockaddr& Server::getTheadress() const {
	return theadress;
}

void Server::setTheadress(const t_sockaddr& theadress) {
	Server::theadress = theadress;
}

int Server::getSockFd() const {
	return _sockFD;
}

void Server::setSockFd(int sockFd) {
	_sockFD = sockFd;
}

const std::string& Server::getPassword() const {
	return _password;
}

void Server::setPassword(const std::string& password) {
	_password = password;
}

const std::string& Server::getServerName() const {
	return _serverName;
}

void Server::setServerName(const std::string& serverName) {
	_serverName = serverName;
}

void Server::serverCleanUp() {
	delete clientsRegistered[BOT_NAME];
	close(_sockFD);
}


void Server::signalHandler(int signal) {
	if (signal == SIGINT)
	{
		serverPointerForSignal->serverCleanUp();
		std::cout << "\nServer is exiting\nByeBye" << std::endl;
		exit(0);
	}
}
