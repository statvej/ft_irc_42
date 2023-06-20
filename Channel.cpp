#include "Channel.hpp"
#include "Server.hpp"

Channel::Channel(const std::string &channel_name, const std::string &creatorname) {
    _channelname = channel_name;
    _flags = 0;
    _user_limit = 0;
    _operators.insert(creatorname);
}

Channel Channel::operator=(Channel c) {
    _channelname = c._channelname;
    _clients_connected = c._clients_connected;
    _operators = c._operators;
    _topic = c._topic;
    _flags = c._flags;
    _voice_priv = c._voice_priv;
    _password = c._password;
    _user_limit = c._user_limit;
    _ban_mask = c._ban_mask;
    _exception_mask = c._exception_mask;
    _invite_mask = c._invite_mask;
    return *this;
}

Channel::Channel(const Channel &c) {
    *this = c;
}

Channel::~Channel() {}

const std::string &Channel::getChannelName() const {
    return _channelname;
}

bool Channel::getClientIsOperator(const std::string &nickname) const {
    return (_operators.count(nickname) != 0);
}

bool Channel::getClientIsConnected(const std::string &nickname) const {
    return (_clients_connected.count(nickname) != 0);
}

const std::string &Channel::getChannelTopic() const {
    return _topic;
}

bool Channel::getChannelFlagIsSet(int flag) const {
    return ((flag & _flags) != 0);
}

bool Channel::getHasVoicePriv(const std::string &nickname) const {
    return (_voice_priv.count(nickname));
}

const std::string &Channel::getChannelPassword() const {
    return _password;
}

size_t Channel::getUserLimit() const {
    return _user_limit;
}

const std::vector <std::string> Channel::getClientsConnected() const {
    std::vector <std::string> clientsConnected;
    for (std::map <std::string, Client>::const_iterator i = _clients_connected.begin(); i != _clients_connected.end(); ++i) {
        clientsConnected.push_back(i->second.getNickname());
    }
    return (clientsConnected);
}

size_t Channel::getNumberClients() const {
    return (_clients_connected.size());
}

bool Channel::getUserIsBanned(Client client) const {
    for (std::set <std::string>::iterator i = _ban_mask.begin() ; i != _ban_mask.end(); ++i) {
        int exc = i->find('!');
        std::string before_exc = i->substr(0, exc);
        int gav = i->find('@');
        std::string before_gav = i->substr(exc, gav);
        std::string after_gav = i->substr(gav);
        std::string part_to_check;
        bool matcher0 = match_strings(before_exc, client.getUsername());
        bool matcher1 = match_strings(before_gav, client.getNickname());
        bool matcher2 = match_strings(after_gav, client.getHost());
        if (matcher2 || matcher1 || matcher0)
            return (true);
    }
    return (false);
}

bool Channel::getUserMatchInviteMask(Client client) const {
    for (std::set <std::string>::iterator i = _invite_mask.begin() ; i != _invite_mask.end(); ++i) {
        int exc = i->find('!');
        std::string before_exc = i->substr(0, exc);
        int gav = i->find('@');
        std::string before_gav = i->substr(exc, gav);
        std::string after_gav = i->substr(gav);
        std::string part_to_check;
        bool matcher0 = match_strings(before_exc, client.getUsername());
        bool matcher1 = match_strings(before_gav, client.getNickname());
        bool matcher2 = match_strings(after_gav, client.getHost());
        if (matcher2 || matcher1 || matcher0)
            return (true);
    }
    return (false);
}

bool Channel::getUserMatchExceptionMask(Client client) const {
    for (std::set <std::string>::iterator i = _exception_mask.begin() ; i != _exception_mask.end(); ++i) {
        int exc = i->find('!');
        std::string before_exc = i->substr(0, exc);
        int gav = i->find('@');
        std::string before_gav = i->substr(exc, gav);
        std::string after_gav = i->substr(gav);
        std::string part_to_check;
        bool matcher0 = match_strings(before_exc, client.getUsername());
        bool matcher1 = match_strings(before_gav, client.getNickname());
        bool matcher2 = match_strings(after_gav, client.getHost());
        if (matcher2 || matcher1 || matcher0)
            return (true);
    }
    return (false);
}
bool Channel::getIsPassword() const {
    return (!_password.empty());
}

bool Channel::getCLientIsInvited(const std::string &nickname) {
    if (_clients_invited.find(nickname) != _clients_invited.end())
        return true;
    return false;
}

void Channel::setClientOperator(const std::string &nickname) {
    _operators.insert(nickname);
}

void Channel::unsetClientOperator(const std::string &nickname) {
    _operators.erase(nickname);
}

void Channel::setClientInvited(const std::string &nickname) {
    _clients_invited.insert(nickname);
}

void Channel::setChannelTopic(const std::string &topic) {
    _topic = topic;
}

void Channel::toggleFlag(int flag) {
    _flags ^= flag;
}

void Channel::setVoicePriv(const std::string &nickname) {
    _voice_priv.insert(nickname);
}

void Channel::setPassword(const std::string &password) {
    _password = password;
}

void Channel::setUserLimit(int user_limit) {
    _user_limit = user_limit;
}

void Channel::setBanMask(const std::string &ban_mask) {
    _ban_mask.insert(ban_mask);
}

void Channel::setExceptionMask(const std::string &exception_mask) {
    _exception_mask.insert(exception_mask);
}

void Channel::setInviteMask(const std::string &invite_mask) {
    _invite_mask.insert(invite_mask);
}

void Channel::setClientIsNotInvited(const std::string &nickname) {
    _clients_invited.erase(nickname);
}

void Channel::joinChannel(Client client) {
    _clients_connected.insert(std::pair<std::string, Client>(client.getNickname(), client));
}

void Channel::sendMessage(Server &server, const Client &client, std::string text, bool skip_self) {
    for (std::map <std::string, Client>::iterator i = _clients_connected.begin(); i != _clients_connected.end(); ++i) {
        if (skip_self && client.getNickname() == i->second.getNickname()) {
            continue;
        }
        server.sendMessage(&i->second, MSG_PRIVMSG_TEMPLATE(client.getPrefix(), _channelname, text));
    }
}

void Channel::kickFromChannel(Server &server, const Client &client, const std::string &kick_mess) {
    sendMessage(server, client, MSG_KICK(_channelname, kick_mess), false);
    _clients_connected.erase(client.getNickname());
}

void Channel::partFromChannel(Server &server, const Client &client, const std::string &kick_mess) {
    sendMessage(server, client, MSG_PART(_channelname, kick_mess), false);
    _clients_connected.erase(client.getNickname());
}

const std::set <std::string> &Channel::getBanmask() const {
    return (_ban_mask);
}

const std::set <std::string> &Channel::getExceptionmask() const {
    return (_exception_mask);
}

const std::set <std::string> &Channel::getInvitemask() const {
    return (_invite_mask);
}

void Channel::unsetBanmask(const std::string &banmask) {
    _ban_mask.erase(banmask);
}

void Channel::unsetInvitemask(const std::string &invitemask) {
    _invite_mask.erase(invitemask);
}

void Channel::unsetExceptionmask(const std::string &exceptionmask) {
    _exception_mask.erase(exceptionmask);
}

bool Channel::empty() const {
    return _clients_connected.empty();
}
