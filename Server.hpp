#include <ostream>
#include "Client.hpp"
#include "irc.hpp"
#include "Message.hpp"
#include "Channel.hpp"
#include "ChatBot.hpp"

#ifndef SERVER_HPP
# define SERVER_HPP

typedef struct sockaddr_in t_sockaddr;

class Server
{
private:
    std::map <int, Client> clients_connected;
    std::map <std::string, Client*> clientsRegistered;
    fd_set _readFDs;
    fd_set _writeFDs;
    t_sockaddr theadress;
    int _sockFD;
    std::string _password;
    std::string _serverName;
    std::map <std::string, Channel*> _channels;
public:

    static Server *serverPointerForSignal;
    void serverCleanUp();
    static void signalHandler(int signal);

public:
    const std::map<int, Client> &getClientsConnected() const;

    void setClientsConnected(const std::map<int, Client> &clientsConnected);

    const std::map<int, Client&> &getClientsRegistered() const;

    void setClientsRegistered(const std::map<int, Client&> &clientsRegistered);

    const fd_set &getReadFDs() const;

    void setReadFDs(const fd_set &readFDs);

    const t_sockaddr &getTheadress() const;

    void setTheadress(const t_sockaddr &theadress);

    int getSockFd() const;

    void setSockFd(int sockFd);

    const std::string &getPassword() const;

    void setPassword(const std::string &password);

    const std::string &getServerName() const;

    void setServerName(const std::string &serverName);
public:
    Server(int port, std::string password, std::string key);
    ~Server();

    void connectClient(int fd);
    void disconectClient(Client &cl);
    bool clientIsConnected(Client &cl);
    bool clientIsRegistered(Client& cl);
    bool clientIsRegistered(std::string str);
    void processMessage(char *rawMessage, Client *client);
    void sendMessage(Client *cl, std::string msg);
    void sendMessageToAll(std::string msg);
    void loop();
    void checkAndRegister(Client *cl);

    //COMANDS
public:
    void PASS(Client *client, Message message);
    void NICK(Client *client, Message message);
    void USER(Client *client, Message message);
    void PRIVMSG(Client *client, Message message);
    void QUIT(Client *client, Message message);
    void JOIN(Client *clinet, Message message);
    void PART(Client *client, Message message);
    void TOPIC(Client *client, Message message);
    void INVITE(Client *client, Message message);
    void NAMES(Client *client, Message message);
    void LIST(Client *client, Message message);
    void KICK(Client *client, Message message);
    void MODE(Client *client, Message message);
    void PING(Client *client, Message message);
};



#endif
