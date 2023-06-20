#include "irc.hpp" 
//PASS
#define ERR_NEEDMOREPARAMS(command) ("461 " + command + " :Not enough parameters\r\n")
#define MSG_PASS_SUCCESS "You passed successfuly\r\n"

//NICK
#define ERR_ALREADYREGISTRED "462 :You may not reregister\r\n"
#define ERR_NONICKNAMEGIVEN "431 :No nickname given\r\n"
#define ERR_ERRONEUSNICKNAME(nick) ("432 " + nick +" :Erroneus nickname\r\n")
#define ERR_NICKNAMEINUSE(nick) ("433 " + nick + " :Nickname is already in use")
#define MSG_NICK_SUCCESS(nick) ("Your nick has been succesfully set to " + nick + "\r\n")
#define ERR_NICKCOLLISION(nick, user, host) "436 " + nick + " :Nickname collision KILL from " + user + "@" + host + "\r\n"


//USER
#define RPL_WELCOME(nick, user, host) "001 :Welcome to the Internet Relay Network " + nick + "!" + user + "@" + host + "\r\n"
#define MSG_REGISTER_SUCCESS  ":server.name 001 yourNickname :Welcome to the IRC Network yourNickname!user@host\r\n"
#define MSG_USER_SUCCESS "Your username and real name got set succesfuly\r\n"

//QUIT
#define MSG_QUIT_CUSTOM(prefix, message) prefix + " QUIT :" + message + "\r\n"
#define MSG_QUIT_DEFAULT(prefix) prefix + " QUIT : *insert default quit message*\r\n"

//KICK
#define MSG_KICK(channel, message) ("KICK " + channel + " :" + message + "\r\n")

//PART
#define MSG_PART(channel, message) ("PART " + channel + " :" + message + "\r\n")

//MODE
#define RPL_CHANNELMODEIS(channel, mode, mode_params) ("324 " + channel + " " + mode + " " + mode_params + "\r\n")

//CHANNEL
#define ERR_USERNOTINCHANNEL(nick, channel) "441 " + nick + " " + channel + " :They aren't on that channel\r\n"
#define ERR_NOTONCHANNEL(channel_name) "442 " + channel_name + " :You're not on that channel\r\n"
#define ERR_USERONCHANNEL(user, channel) "443 " + user + " " + channel + " :is already on channel\r\n"
#define ERR_NOLOGIN(user) "444 " + user + " :User not logged in\r\n"
#define ERR_SUMMONDISABLED "445 :SUMMON has been disabled\r\n"
#define ERR_USERSDISABLED "446 :USERS has been disabled\r\n"

#define ERR_NOTREGISTERED "451 :You have not registered\r\n"

#define ERR_NOPERMFORHOST "463 :Your host isn't among the privileged\r\n"
#define ERR_PASSWDMISMATCH "464 :Password incorrect\r\n"
#define ERR_YOUREBANNEDCREEP "465 :You are banned from this server\r\n"
#define ERR_YOUWILLBEBANNED "466\r\n"

#define RPL_NOTOPIC(channel_name) "331 " + channel_name + " :No topic is set\r\n"
#define RPL_TOPIC(channel_name, topic) "332 " + channel_name + " :" + topic + "\r\n"

//NO SUCH SMTH
#define ERR_NOSUCHNICK(nick) ("401 " + nick + " :No such nick/channel\r\n")
#define ERR_NOSUCHSERVER(server_name) "402 " + server name + " :No such server\r\n"
#define ERR_NOSUCHCHANNEL(channel_name) "403 " + channel_name + " :No such channel\r\n"
#define ERR_CANNOTSENDTOCHAN(channel_name) "404 " +channel_name + " :Cannot send to channel\r\n"
#define ERR_TOOMANYCHANNELS(channel_name) "405 " + channel_name + " :You have joined too many channels"
#define ERR_WASNOSUCHNICK(nickname) "406 " + nickname + " :There was no such nickname"
#define ERR_TOOMANYTARGETS(target, error_code, abort_message) "407 " + target + " :" + error_code + " recipients. " + abort_message + "\r\n"
#define ERR_NOSUCHSERVICE(service_name) "408 " + service_name + " :No such service"
#define ERR_NOORIGIN "409 :No origin specified\r\n"
#define ERR_NORECIPIENT(command) "411 :No recipient given (" + command + ")\r\n"
#define ERR_NOTEXTTOSEND "412 :No text to send\r\n"
#define ERR_NOTOPLEVEL(mask) "413 " +mask + " :No toplevel domain specified\r\n"
#define ERR_WILDTOPLEVEL(mask) "414 " + mask + " :Wildcard in toplevel domain\r\n"
#define ERR_BADMASK(mask) "415 " + mask + " :Bad Server/host mask\r\n"

//NOT ENOUGH INFO
#define ERR_UNKNOWNCOMMAND(command) "421 " + command + " :Unknown command\r\n"
#define ERR_NOMOTD "422 :MOTD File is missing\r\n"
#define ERR_NOADMININFO(server) "423 " + server + " :No administrative info available\r\n"
#define ERR_FILEERROR(file_op, file) "424 :File error doing " + file_op + " on " + file + "\r\n"

#define ERR_UNAVAILRESOURCE(target) "437 " + target + " :Nick/channel is temporarily unavailable\r\n"

//FLAGS
#define ERR_KEYSET(channel) "467 " + channel + " :Channel key already set\r\n"
#define ERR_CHANNELISFULL(channel) "471 " + channel + " :Cannot join channel (+l)\r\n"
#define ERR_UNKNOWNMODE(ch, channel) "472 " + ch + " :is unknown mode char to me for " + channel + "\r\n"
#define ERR_INVITEONLYCHAN(channel) "473 " + channel + " :Cannot join channel (+i)\r\n"
#define ERR_BANNEDFROMCHAN(channel) "474 " + channel + " :Cannot join channel (+b)\r\n"
#define ERR_BADCHANNELKEY(channel) "475 " + channel + " Cannot join channel (+k)\r\n"
#define ERR_BADCHANMASK(channel) "476 " + channel + " :Bad Channel Mask\r\n"
#define ERR_NOCHANMODES(channel) "477 " + channel + " :Channel doesn't support modes\r\n"
#define ERR_BANLISTFULL(channel, ch) "478 " + channel + " " + ch + " :Channel list is full\r\n"

//PERMISSIONS
#define ERR_NOPRIVILEGES "481 :Permission Denied- You're not an IRC operator\r\n"
#define ERR_CHANOPRIVSNEEDED(channel) "482 " + channel + " :You're not channel operator\r\n"
#define ERR_CANTKILLSERVER "483 :You can't kill a server!\r\n"
#define ERR_RESTRICTED "484 :Your connection is restricted!\r\n"
#define ERR_UNIQOPPRIVSNEEDED "485 :You're not the original channel operator\r\n"

#define ERR_NOOPERHOST "491 :No O-lines for your host\r\n"
#define ERR_UMODEUNKNOWNFLAG "501 :Unknown MODE flag\r\n"
#define ERR_USERSDONTMATCH "502 :Cannot change mode for other users\r\n"

#define RPL_INVITING(channel, nick) "341 " + channel + " " + nick + "\r\n"
#define RPL_INVITE_RECEIVER(sender, recipient, channel) ":" + sender + " INVITE " + recipient + " " + channel + "\r\n"

#define RPL_NAMREPLY(client, symbol, channel, nicks) "353 " + client + " " + symbol + " " + channel + " :" + nicks + "\r\n"
#define RPL_ENDOFNAMES(client, channel) ("366 " + client + " " + channel + " :End of /NAMES list\r\n")
#define RPL_LIST(channel, count_client, topic) ("322 " + channel + " " + count_client + " :" + topic + "\r\n")
#define RPL_LISTEND "323 :End of LIST\r\n"

#define ERR_INVALIDKEY(client, channel) ("525 " + client + " " + channel + " :Key is not well-formed\r\n")


#define RPL_INVITELIST(channel, banmask) ("346 " + channel + " " + banmask + "\r\n")
#define RPL_ENDOFINVITELIST(channel) ("347 " + channel + " :End of channel invite list\r\n")

#define RPL_EXCEPTLIST(channel, exceptionmask) ("348 "+ channel + " " + exceptionmask + "\r\n")
#define RPL_ENDOFEXCEPTLIST(channel) ("349 " + channel + " :End of channel exception list\r\n")

#define RPL_BANLIST(channel, banmask) ("367 " + channel + " " + banmask + "\r\n")
#define RPL_ENDOFBANLIST(channel) ("368 " + channel + " :End of channel ban list\r\n")


#define RPL_MOTDSTART(nick) (":better_than_slack 375 " + nick + " :- " + HOST_NAME + " Message of the day - \r\n")
#define RPL_MOTD(nick) (":better_than_slack 372 " + nick + " :" + MOTD + "\r\n")
#define RPL_MOTDEND(nick) (":better_than_slack 376 " + nick + " :End of /MOTD command.\r\n")
#define RPL_PONG(host_name, token) (":" + host_name + " PONG " + host_name + " :" token + "\r\n")


std::string MSG_PRIVMSG_TEMPLATE(std::string prefix, std::string to, std::string text);
