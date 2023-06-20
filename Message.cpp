#include "Message.hpp"


Message::Message()
    : _prefix(""), _command(""), _parameters(std::vector<std::string>())
{
}

Message::~Message()
{
}

Message::Message(char* in_str) {
    std::string act_str(in_str);
    if (!act_str.empty())
        parseMessage(act_str);
}

Message::Message(const std::string &mes) {
    if (!mes.empty())
        parseMessage(mes);
}

std::string Message::getPrefix() const {
    return _prefix;
}
std::string Message::getCommand() const {
    return _command;
}
std::vector <std::string> Message::getParameters() const {
    return _parameters;
}

std::string Message::getFirstParam() const{
    return _parameters[0];
}

void Message::parseMessage(std::string raw_message) {
    //in case of prefix
    raw_message.erase(raw_message.find("\r\n"), std::string::npos);
    if (raw_message[0] == ':')
    {
        _prefix = removeNonPrintableCharacters(raw_message.substr(1, raw_message.find(' ')));
        raw_message.erase(0, raw_message.find(' ') + 1);
    }
    _command = removeNonPrintableCharacters(raw_message.substr(0, raw_message.find(' ')));
    if (raw_message.find(' ') == std::string::npos) {
        return;
    }
    raw_message.erase(0, raw_message.find(' ') + 1);
    size_t pos;
    while ((pos = raw_message.find(' ')) != raw_message.npos && raw_message[0] != ':')
    {
        _parameters.push_back(raw_message.substr(0, pos));
        raw_message = raw_message.erase(0, pos + 1);
    }
    //in case of postfix
    if (raw_message[0] == ':')
        raw_message = raw_message.erase(0, 1);
    _parameters.push_back(raw_message);
}

void Message::setPrefix(const std::string &prefix) {
    _prefix = prefix;
}

void Message::setCommand(const std::string &command) {
    _command = command;
}

void Message::setParameters(const std::vector<std::string> &parameters) {
    _parameters = parameters;
}

std::ostream& operator<<(std::ostream& o, Message const& m)
{
    std::cout << "PREFIX: " << m.getPrefix() << "; COMMAND: " << m.getCommand() << "; PARAMS: [";
    std::vector <std::string> params(m.getParameters());
    for (std::vector<std::string>::iterator i = params.begin(); i < params.end(); i++)
        std::cout << ", " << *i;
    std::cout << "]";
    return (o);
}
