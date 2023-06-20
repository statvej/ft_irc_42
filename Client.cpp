
#include "Client.hpp"

Client::Client(/* args */)
{
}

Client::Client(int fd)
    : _sockFD(fd)
{
}

Client::~Client()
{
}

Client& Client::operator=(Client const& other) {
    (*this)._nickname = other.getNickname();
    (*this)._username = other.getUsername();
    (*this)._realname = other.getRealname();
    (*this)._usermode = other.getUsermode();
    setApiKey(other.getApiKey());
    setPrefix(other.getPrefix());
    (*this)._messageBuffer = other.getMessageBuffer();
    (*this)._sockFD = other.getSockFD();
    return *this;
}

Client::Client(Client const& other) {
    *this = other;
}


int Client::getSockFD()const
{
    return _sockFD;
}

const std::string& Client::getNickname() const {
    return _nickname;
}

void Client::setNickname(const std::string& nickname) {
    _nickname = nickname;
}

const std::string& Client::getUsername() const {
    return _username;
}

void Client::setUsername(const std::string& username) {
    _username = username;
}

const std::string& Client::getPrefix() const {
    return _prefix;
}

void Client::setPrefix(const std::string& prefix) {
    _prefix = prefix;
}

void Client::setPrefix(const std::string& nick, const std::string& user, const std::string& host) {
    _prefix = ":" + nick + "!" + user + "@127.0.0.1";
    _host = host;
}

const std::string& Client::getRealname() const {
    return _realname;
}

void Client::setRealname(const std::string& realname) {
    _realname = realname;
}

int Client::getUsermode() const {
    return _usermode;
}

void Client::setUsermode(int usermode) {
    _usermode = usermode;
}

std::string Client::getMessageBuffer() const {
    return _messageBuffer;
}

void Client::setMessageBuffer(const std::string& messageBuffer) {
    _messageBuffer = messageBuffer;
}

int Client::getSockFd() const {
    return _sockFD;
}

void Client::setSockFd(int sockFd) {
    _sockFD = sockFd;
}

std::string Client::getHost()const {
    return _host;
}

void Client::setHost(std::string set) {
    _host = set;
}

std::ostream& operator<<(std::ostream& os, const Client& client) {
    os << "_nickname: " << client._nickname << " _username: " << client._username << " _realname: " << client._realname
        << " _usermode: " << client._usermode << " _messageBuffer: " << client._messageBuffer << " _sockFD: "
        << client._sockFD;
    return os;
}

void Client::sendMessage(std::string message) {
    send(_sockFD, message.c_str(), message.length(), 0);
}

void Client::addToBuffer(std::string to_add) {
    _messageBuffer += to_add;
}

void Client::flushBuffer() {
    _messageBuffer.erase(0, _messageBuffer.find("\r\n") + 2);
}

std::string Client::getApiKey() const {
    return apiKey;
}

void Client::setApiKey(std::string key)
{
    apiKey = key;
}
