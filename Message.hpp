
#include "irc.hpp"

#ifndef MESSAGE_HPP
# define MESSAGE_HPP

class Message
{
private:
    std::string _prefix;
    std::string _command;
    std::vector <std::string> _parameters;
public:
    void setPrefix(const std::string &prefix);

    void setCommand(const std::string &command);

    void setParameters(const std::vector<std::string> &parameters);

public:
    Message(/* args */);
    Message(const std::string &mes);
    Message(char *);
    ~Message();

    std::string getPrefix() const;
    std::string getCommand() const;
    std::vector <std::string> getParameters() const;
    std::string getFirstParam() const;

    //parse messsage from string
    void parseMessage(std::string raw_message); 
};

std::ostream &operator<<(std::ostream &o, Message const &m);



#endif
