#ifndef CLIENT_HPP 
# define CLIENT_HPP

#include <ostream>
#include "irc.hpp"

#include "Message.hpp"

enum USER_MODE {
    CONNECTED,
    PASSED,
    REGISTERED,
    OPERATOR,
    BOT
};

class Message;

class Client
{
private:
    std::string _nickname;
    std::string _username;
    std::string _realname;
    std::string _prefix;
    std::string _host;
    int _usermode;
    std::string _messageBuffer;
    std::string apiKey;
    int _sockFD;
public:
    const std::string& getNickname() const;

    friend std::ostream& operator<<(std::ostream& os, const Client& client);

    void setNickname(const std::string& nickname);

    const std::string& getUsername() const;

    void setUsername(const std::string& username);

    const std::string& getPrefix() const;

    void setPrefix(const std::string& prefix);

    void setPrefix(const std::string& nick, const std::string& user, const std::string& host);

    const std::string& getRealname() const;

    void setRealname(const std::string& realname);

    int getUsermode() const;

    void setUsermode(int usermode);

    std::string getMessageBuffer() const;

    void setMessageBuffer(const std::string& messageBuffer);

    int getSockFd() const;

    void setSockFd(int sockFd);

    std::string getApiKey() const;

    void setApiKey(std::string key);

    std::string getHost() const;

    void setHost(std::string set);

    void addToBuffer(std::string to_add);

    void flushBuffer();

public:
    Client();
    Client(int);
    Client(Client const& other);
    Client& operator=(Client const& other);
    ~Client();
    int getSockFD()const;
    void sendMessage(std::string message);
};

#endif
