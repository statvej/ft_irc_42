#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include "Client.hpp"
#include "irc.hpp"
#include "Utils.hpp"
#include <regex>
#include <set>
#include <map>

#define CHANNEL_MODES_ANONYMOUS 1 //a flag
#define CHANNEL_MODES_INVITE_ONLY 2 //i flag
#define CHANNEL_MODES_MODERATED 4 //m flag
#define CHANNEL_MODES_NO_OUTSIDE 8 //n flag
#define CHANNEL_MODES_QUIET 16 //q flag
#define CHANNEL_MODES_PRIVATE 32 //p flag
#define CHANNEL_MODES_SECRET 64 //s flag
#define CHANNEL_MODES_REOP 128 //r flag
#define CHANNEL_MODES_TOPIC_OPERATOR 256 //t flag

class Server;

class Channel
{
    private:
        std::string _channelname;
        std::map <std::string, Client> _clients_connected;
        std::set<std::string> _clients_invited;
        std::set<std::string> _operators; 
        std::string _topic;
        int _flags;
        std::set<std::string> _voice_priv;
        std::string _password; //k flag
        int _user_limit; //l flag
        std::set <std::string> _ban_mask;
        std::set <std::string> _exception_mask;
        std::set <std::string> _invite_mask;
        Channel();


    public:
        Channel(const std::string &channel_name, const std::string &creatorname);
        Channel operator=(Channel c);
        Channel(const Channel &c);
        ~Channel();
        const std::string &getChannelName() const;
        bool getClientIsOperator(const std::string &nickname) const;
        bool getClientIsConnected(const std::string &nickname) const;
        const std::string &getChannelTopic() const;
        bool getChannelFlagIsSet(int flag) const;
        bool getHasVoicePriv(const std::string &nickname) const;
        const std::string &getChannelPassword() const;
        size_t getUserLimit() const;
        const std::vector <std::string> getClientsConnected() const;
        size_t getNumberClients() const;
        bool getUserIsBanned(Client client) const;
        bool getUserMatchInviteMask(Client client) const;
        bool getUserMatchExceptionMask(Client client) const;
        bool empty() const;

        bool getIsPassword() const;
        bool getCLientIsInvited(const std::string &nickname);
        const std::set <std::string> &getBanmask() const;
        const std::set <std::string> &getInvitemask() const;
        const std::set <std::string> &getExceptionmask() const;

        void setClientOperator(const std::string &nickname);
        void unsetClientOperator(const std::string &nickname);
        void setClientInvited(const std::string &nickname);
        void setChannelTopic(const std::string &topic);
        void toggleFlag(int flag);
        void setVoicePriv(const std::string &nickname);
        void setPassword(const std::string &password);
        void setUserLimit(int user_limit);
        void setBanMask(const std::string &ban_mask);
        void setExceptionMask(const std::string &exception_mask);
        void setInviteMask(const std::string &invite_mask);
        void setClientIsNotInvited(const std::string &nickname);

        void joinChannel(Client client);
        void deleteFromChannel(Client client, const std::string &part_mess);
        void partFromChannel(Server &server, const Client &client, const std::string &part_mess);
        void kickFromChannel(Server &server, const Client &client, const std::string &kick_mess);
        void sendMessage(Server &server, const Client &client, std::string text, bool skip_self);
        void unsetBanmask(const std::string &banmask);
        void unsetInvitemask(const std::string &invitemask);
        void unsetExceptionmask(const std::string &exceptionmask);
};

#endif
