//
// Created by Fedir Staryk on 4/25/23.
//

#ifndef FT_IRC_CHATBOT_HPP
#define FT_IRC_CHATBOT_HPP
#include "irc.hpp"
#include "Client.hpp"
#include "Server.hpp"
#include <curl/curl.h>


// class Message;
class ChatBot: public Client {

private:
public:
    ChatBot();
    ChatBot(std::string key);
    ~ChatBot();
    ChatBot(Client const &other);
    ChatBot &operator=(ChatBot const &other);
    void parseBotCommand(std::string message, Client *sender);
    std::string getChatGPTResponse(std::string request);
    std::string trimResponse(std::string &response);
};



#endif //FT_IRC_CHATBOT_HPP
