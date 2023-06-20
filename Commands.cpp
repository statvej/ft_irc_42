#include "Server.hpp"
#include <vector>
void Server::PASS(Client* client, Message message) {

	//wrong number of args
	if (message.getParameters().size() != 1)
		return sendMessage(client, ERR_NEEDMOREPARAMS(std::string("PASS")));
	if (message.getFirstParam() == _password)
	{
		sendMessage(client, MSG_PASS_SUCCESS);
		client->setUsermode(PASSED);
	}
	else
		sendMessage(client, ERR_ALREADYREGISTRED);
}

/*2 cases
	1.Just set a nickname(need to check if it exists)

	2.If we get a prefix we change nick from prefix to arg
*/
void Server::NICK(Client* client, Message message) {
	if (message.getParameters().size() == 0)
		return sendMessage(client, ERR_NONICKNAMEGIVEN);
	if (!strIsASCII(message.getFirstParam()))
		return sendMessage(client, ERR_ERRONEUSNICKNAME(message.getFirstParam()));
	if (message.getPrefix().empty() && clientIsRegistered(message.getFirstParam()))
		return sendMessage(client, ERR_NICKNAMEINUSE(message.getFirstParam()));
	if (!message.getPrefix().empty())
		clientsRegistered.erase(clientsRegistered.find(message.getPrefix()));
	if (!clientIsRegistered(client->getNickname()))
	{
		client->setNickname(message.getFirstParam());
	}
	checkAndRegister(client);
	sendMessage(client, MSG_NICK_SUCCESS(client->getNickname()));
}

void Server::USER(Client* client, Message message) {
	if (message.getParameters().size() != 4)
		return sendMessage(client, ERR_NEEDMOREPARAMS(std::string("USER")));
	if (clientIsRegistered(*client))
		return sendMessage(client, ERR_ALREADYREGISTRED);
	client->setUsername(message.getFirstParam());
	client->setRealname(message.getParameters().back());
	sendMessage(client, MSG_USER_SUCCESS);
	checkAndRegister(client);
}

std::string MSG_PRIVMSG_TEMPLATE(std::string prefix, std::string to, std::string text)
{
	std::string msg;
	msg += prefix;
	msg += " PRIVMSG ";
	msg += to + " :";
	msg += text;
	msg += "\r\n";
	return (msg);
}


void Server::PRIVMSG(Client* client, Message message) {
	std::vector <std::string> params = message.getParameters();
	if (params.size() > 2) {
		sendMessage(client, ERR_TOOMANYTARGETS(params[0], "407", "You entered too many parameters"));
		return ;
	}
	if (params.size() == 1) {
		sendMessage(client, ERR_NOTEXTTOSEND);
		return ;
	}
	if (params.size() == 0) {
		sendMessage(client, ERR_NORECIPIENT(std::string("PRIVMSG")));
		return ;
	}
	if (params[0][0] == '#' || _channels[params[0]]) {
		if (_channels.count(params[0]) == 0) {
			sendMessage(client, ERR_NORECIPIENT(std::string("PRIVMSG")));
			return ;
		}
		if ((_channels[params[0]]->getChannelFlagIsSet(CHANNEL_MODES_NO_OUTSIDE) && !_channels[params[0]]->getClientIsConnected(client->getNickname())) ||
				(_channels[params[0]]->getChannelFlagIsSet(CHANNEL_MODES_MODERATED) && !_channels[params[0]]->getClientIsOperator(client->getNickname())) ||
				_channels[params[0]]->getUserIsBanned(*client)) {
			sendMessage(client, ERR_CANNOTSENDTOCHAN(params[0]));
			return ;
		}
		std::string channel_to_send = params[0];
		std::string text_to_send = params[1];
		_channels[channel_to_send]->sendMessage(*this, *client, text_to_send, true);
		return ;
	}

	for (size_t i = 0; i < message.getParameters().size() - 1; i++)
	{
		if (clientIsRegistered(message.getParameters()[i])) {
			if (clientsRegistered[message.getParameters()[i]]->getNickname() == BOT_NAME) {
				ChatBot botInstance(*clientsRegistered[message.getParameters()[i]]);
				botInstance.parseBotCommand(message.getParameters().back(), client);
			}
			else
				sendMessage(clientsRegistered[message.getParameters()[i]], MSG_PRIVMSG_TEMPLATE(client->getPrefix(), \
					message.getParameters()[i], \
					message.getParameters().back()));
		}
	}
}

void Server::QUIT(Client* client, Message message) {
	if (message.getParameters().empty())
		sendMessageToAll(MSG_QUIT_DEFAULT(client->getPrefix()));
	else
		sendMessageToAll(MSG_QUIT_CUSTOM(client->getPrefix(), message.getFirstParam()));
	disconectClient(*client);
}

std::string PONGREPLY(std::string token)
{
	std::string msg;
	msg += ":" + std::string(HOST_NAME);
	msg += " PONG ";
	msg += std::string(HOST_NAME);
	msg += " :" + token;
	msg += "\r\n";
	return (msg);
}

void Server::PING(Client *client, Message message){
	if (!message.getParameters().size())
		sendMessage(client, ERR_NEEDMOREPARAMS(std::string("PING")));
	else
		sendMessage(client, PONGREPLY(message.getFirstParam()));
}


//CHANNELS
void Server::JOIN(Client *client, Message message) {
	std::vector <std::string> params = message.getParameters();
	if (params.size() < 1 || params[0] == "#") {
		sendMessage(client, ERR_NEEDMOREPARAMS(std::string("JOIN")));
		return ;
	}
	std::vector <std::string> splitted_str = split(params[0], ',');
	std::string channel_name = params[0];
	if (channel_name == "0") {
		for (std::map <std::string, Channel *>::iterator i = _channels.begin(); i != _channels.end(); ++i) {
			if (i->second->getClientIsConnected(client->getNickname()) == true) {
				i->second->partFromChannel(*this, *client, "");
			}
		}
		return ;
	}
	std::vector <std::string> splitted_keys;
	if (params.size() == 2) {
		splitted_keys = split(params[1], ',');
		if (splitted_str.size() != splitted_keys.size()) {
			sendMessage(client, ERR_NEEDMOREPARAMS(std::string("JOIN")));
			return ;
		}
	}
	size_t j = -1;
	for (std::vector <std::string>::iterator i = splitted_str.begin(); i < splitted_str.end(); ++i) {
		j++;
		if (_channels.count(*i) == 0)
		{
			Channel *c = new Channel(*i, client->getNickname());
			_channels.insert(std::pair <std::string, Channel*>(*i, c));
			_channels[*i]->joinChannel(*client);
			sendMessage(client, RPL_TOPIC(*i, _channels[*i]->getChannelTopic()));
			Message current_mes("NAMES " + *i + "\r\n");
			NAMES(client, current_mes);
			continue;
		}
		if (_channels[*i]->getChannelFlagIsSet(CHANNEL_MODES_INVITE_ONLY) && !(_channels[*i]->getUserMatchInviteMask(*client)) && !_channels[*i]->getCLientIsInvited(client->getNickname())) {
			sendMessage(client, ERR_INVITEONLYCHAN(*i));
			continue;
		}
		if (_channels[*i]->getNumberClients() == _channels[*i]->getUserLimit()) {
			sendMessage(client, ERR_CHANNELISFULL(*i));
			continue;
		}
		if (!(_channels[*i]->getChannelPassword().empty()) && (splitted_keys.size() == 0 || _channels[*i]->getChannelPassword() != splitted_keys[j])) {
			sendMessage(client, ERR_BADCHANNELKEY(*i));
			continue;
		}
		if (_channels[*i]->getUserIsBanned(*client) && !(_channels[*i]->getUserMatchExceptionMask(*client))) {
			sendMessage(client, ERR_BANNEDFROMCHAN(*i));
			continue;
		}
		_channels[*i]->joinChannel(*client);
		sendMessage(client, RPL_TOPIC(*i, _channels[*i]->getChannelTopic()));
		Message current_mes("NAMES " + *i + "\r\n");
		NAMES(client, current_mes);
		if (_channels[*i]->getCLientIsInvited(client->getNickname())) {
			_channels[*i]->setClientIsNotInvited(client->getNickname());
		}

	}
}

void Server::PART(Client *client, Message message) {
	std::vector <std::string> params = message.getParameters();
	if (params.size() < 1) {
		sendMessage(client, ERR_NEEDMOREPARAMS(std::string("PART")));
		return ;
	}
	std::vector <std::string> splitted_str = split(params[0], ',');
	std::string part_mess = client->getNickname();
	if (params.size() > 1) {
		part_mess = params[1];
	}
	for (std::vector <std::string>::iterator i = splitted_str.begin(); i < splitted_str.end(); i++) {
		if (_channels.count(*i) == 0) {
			sendMessage(client, ERR_NOSUCHCHANNEL(*i));
			continue;
		}
		if (_channels[*i]->getClientIsConnected(client->getNickname()) != true) {
			sendMessage(client, ERR_NOTONCHANNEL(*i));
			continue;
		}
		_channels[*i]->partFromChannel(*this, *client, part_mess);
		if (_channels[*i]->empty()) {
			delete _channels[*i];
			_channels.erase(*i);
		}
	}
}

void Server::TOPIC(Client *client, Message message) {
	std::vector <std::string> params = message.getParameters();
	if (params.size() < 1) {
		sendMessage(client, ERR_NEEDMOREPARAMS(std::string("TOPIC")));
		return ;
	}
	if (_channels.count(params[0]) == 0 || _channels[params[0]]->getClientIsConnected(client->getNickname()) == false) {
		sendMessage(client, ERR_NOTONCHANNEL(params[0]));
		return ;
	}
	if (params.size() == 1) {
		if (_channels[params[0]]->getChannelTopic().empty()) {
			sendMessage(client, RPL_NOTOPIC(params[0]));
		}
		else {
			sendMessage(client, RPL_TOPIC(params[0], _channels[params[0]]->getChannelTopic()));
		}
	}
	else {
		if (_channels[params[0]]->getChannelFlagIsSet(CHANNEL_MODES_TOPIC_OPERATOR) && (_channels[params[0]]->getClientIsOperator(client->getNickname()) == false)) {
			sendMessage(client, ERR_CHANOPRIVSNEEDED(params[0]));
			return ;
		}
		_channels[params[0]]->setChannelTopic(params[1]);
		if (params[1].empty()) {
			sendMessage(client, RPL_NOTOPIC(params[0]));
		}
		else {
			sendMessage(client, RPL_TOPIC(params[0], _channels[params[0]]->getChannelTopic()));
		}
	}
}

void Server::INVITE(Client *client, Message message) {
	std::vector <std::string> params = message.getParameters();
	if (params.size() < 2) {
		sendMessage(client, ERR_NEEDMOREPARAMS(std::string("INVITE")));
		return ;
	}
	if (clientsRegistered.count(params[0]) == 0) {
		sendMessage(client, ERR_NOSUCHNICK(params[0]));
		return ;
	}
	if (_channels.count(params[1]) == 0) {
		sendMessage(client, ERR_NOSUCHCHANNEL(params[1]));
		return ;
	}
	if (_channels.count(params[1]) != 0) {
		if (_channels[params[1]]->getClientIsConnected(client->getNickname()) == false) {
			sendMessage(client, ERR_NOTONCHANNEL(params[1]));
			return ;
		}
		if (_channels[params[1]]->getClientIsConnected(params[0]) == true) {
			sendMessage(client, ERR_USERONCHANNEL(params[0], params[1]));
			return ;
		}
		if (_channels[params[1]]->getChannelFlagIsSet(CHANNEL_MODES_INVITE_ONLY) == true && _channels[params[1]]->getClientIsOperator(client->getNickname()) == false) {
			sendMessage(client, ERR_CHANOPRIVSNEEDED(params[1]));
			return ;
		}
	}
	_channels[params[1]]->setClientInvited(params[0]);
	sendMessage(client, RPL_INVITING(params[1], params[0]));
	sendMessage(clientsRegistered[params[0]], RPL_INVITE_RECEIVER(client->getNickname(), params[0], params[1]));
}

void Server::NAMES(Client *client, Message message) {
	std::vector <std::string> params = message.getParameters();
	if (params.size() == 0) {
		for (std::map <std::string, Channel *>::iterator i = _channels.begin(); i != _channels.end(); ++i) {
			char symbol = '=';
			if (i->second->getChannelFlagIsSet(CHANNEL_MODES_ANONYMOUS)) {
				sendMessage(client, RPL_ENDOFNAMES(client->getNickname(), i->first));
				continue;
			}
			if (i->second->getChannelFlagIsSet(CHANNEL_MODES_PRIVATE)) {
				symbol = '*';
			}
			if (i->second->getChannelFlagIsSet(CHANNEL_MODES_SECRET)) {
				symbol = '@';
			}
			if (symbol != '=' && (i->second->getClientIsConnected(client->getNickname()) == false)) {
				sendMessage(client, RPL_ENDOFNAMES(client->getNickname(), i->first));
				continue;
			}
			std::vector <std::string> clientsInChannel = i->second->getClientsConnected();
			std::string nicks;
			for (std::vector <std::string>::iterator j = clientsInChannel.begin(); j < clientsInChannel.end(); ++j) {
				nicks += *j;
				nicks += " ";
			}
			sendMessage(client, RPL_NAMREPLY(client->getNickname(), symbol, i->first, nicks));
			sendMessage(client, RPL_ENDOFNAMES(client->getNickname(), i->first));
		}
	}
	else {
		std::vector <std::string> channelsToExplore = split(params[0], ',');
		for (std::vector <std::string>::iterator i = channelsToExplore.begin(); i < channelsToExplore.end(); ++i) {
			char symbol = '=';
			if (_channels.count(*i) == 0) {
				sendMessage(client, ERR_NOSUCHCHANNEL(*i));
				continue;
			}
			if (_channels[*i]->getChannelFlagIsSet(CHANNEL_MODES_ANONYMOUS)) {
				sendMessage(client, RPL_ENDOFNAMES(client->getNickname(), *i));
				continue;
			}
			if (_channels[*i]->getChannelFlagIsSet(CHANNEL_MODES_PRIVATE)) {
				symbol = '*';
			}
			if (_channels[*i]->getChannelFlagIsSet(CHANNEL_MODES_SECRET)) {
				symbol = '@';
			}
			if (symbol != '=' && (_channels[*i]->getClientIsConnected(client->getNickname()) == false)) {
				sendMessage(client, RPL_ENDOFNAMES(client->getNickname(), *i));
				continue;
			}
			std::vector <std::string> clientsInChannel = _channels[*i]->getClientsConnected();
			std::string nicks;
			for (std::vector <std::string>::iterator j = clientsInChannel.begin(); j < clientsInChannel.end(); ++j) {
				nicks += *j;
				nicks += " ";
			}
			sendMessage(client, RPL_NAMREPLY(client->getNickname(), symbol, *i, nicks));
			sendMessage(client, RPL_ENDOFNAMES(client->getNickname(), *i));
		}
	}
}

void Server::LIST(Client *client, Message message) {
	std::vector <std::string> params = message.getParameters();
	if (params.size() == 0) {
		for (std::map <std::string, Channel *>::iterator i = _channels.begin(); i != _channels.end(); ++i) {
			size_t count_client = i->second->getNumberClients();
			sendMessage(client, RPL_LIST(i->first, std::to_string(count_client), i->second->getChannelTopic()));
		}
	}
	else {
		std::vector <std::string> channelsToExplore = split(params[0], ',');
		for (std::vector <std::string>::iterator i = channelsToExplore.begin(); i < channelsToExplore.end(); ++i) {
			if (_channels.count(*i) == 0) {
				sendMessage(client, ERR_NOSUCHCHANNEL(*i));
				continue;
			}
			size_t count_client = _channels[*i]->getNumberClients();
			sendMessage(client, RPL_LIST(*i, std::to_string(count_client), _channels[*i]->getChannelTopic()));
		}
	}
	sendMessage(client, RPL_LISTEND);
}

void Server::KICK(Client *client, Message message) {
	std::vector <std::string> params = message.getParameters();
	if (params.size() < 2) {
		sendMessage(client, ERR_NEEDMOREPARAMS(std::string("JOIN")));
		return ;
	}
	std::vector <std::string> splitted_channs = split(params[0], ',');
	std::vector <std::string> splitted_users = split(params[1], ',');
	std::string comment = client->getNickname();
	if (params.size() == 3) {
		comment = params[2];
	}
	if (splitted_channs.size() != splitted_users.size() && splitted_channs.size() != 1) {
		sendMessage(client, ERR_NEEDMOREPARAMS(std::string("JOIN")));
		return ;
	}
	if (splitted_channs.size() > 1) {
		for (size_t i = 0; i < splitted_channs.size(); ++i) {
			if (_channels.count(splitted_channs[i]) == 0) {
				sendMessage(client, ERR_NOSUCHCHANNEL(splitted_channs[i]));
				continue;
			}
			if (!(_channels[splitted_channs[i]]->getClientIsConnected(client->getNickname()))) {
				sendMessage(client, ERR_NOTONCHANNEL(splitted_channs[i]));
				continue;
			}
			if (!(_channels[splitted_channs[i]]->getClientIsOperator(client->getNickname()))) {
				sendMessage(client, ERR_CHANOPRIVSNEEDED(splitted_channs[i]));
				continue;
			}
			if (!(_channels[splitted_channs[i]]->getClientIsConnected(splitted_users[i]))) {
				sendMessage(client, ERR_USERNOTINCHANNEL(splitted_users[i], splitted_channs[i]));
				continue;
			}
			_channels[splitted_channs[i]]->kickFromChannel(*this, *clientsRegistered[splitted_users[i]], comment);
		}
		return;
	}
	for (size_t i = 0; i < splitted_users.size(); ++i) {
		if (_channels.count(splitted_channs[i]) == 0) {
			sendMessage(client, ERR_NOSUCHCHANNEL(splitted_channs[i]));
			continue;
		}
		if (!(_channels[splitted_channs[0]]->getClientIsConnected(client->getNickname()))) {
			sendMessage(client, ERR_NOTONCHANNEL(splitted_channs[i]));
			continue;
		}
		if (!(_channels[splitted_channs[0]]->getClientIsOperator(client->getNickname()))) {
			sendMessage(client, ERR_CHANOPRIVSNEEDED(splitted_channs[i]));
			continue;
		}
		if (!(_channels[splitted_channs[0]]->getClientIsConnected(splitted_users[i]))) {
			sendMessage(client, ERR_USERNOTINCHANNEL(splitted_users[i], splitted_channs[i]));
			continue;
		}
		_channels[splitted_channs[0]]->kickFromChannel(*this, *clientsRegistered[splitted_users[i]], comment);
	}
}


void Server::MODE(Client *client, Message message) {
	std::vector <std::string> params = message.getParameters();
	std::string modes_to_print = "+";

	if (params.size() < 1) {
		sendMessage(client, ERR_NEEDMOREPARAMS(std::string("MODE")));
		return ;
	}
	if (_channels.count(params[0]) == 0) {
		sendMessage(client, ERR_NOSUCHCHANNEL(params[0]));
		return ;
		}
	if (!_channels[params[0]]->getClientIsConnected(client->getNickname())) {
		sendMessage(client, ERR_USERNOTINCHANNEL(client->getNickname(), params[0]));
		return ;
	}

	if (params.size() == 1) {
		std::string mode_params;
		if (_channels[params[0]]->getChannelFlagIsSet(CHANNEL_MODES_ANONYMOUS))
			modes_to_print += "a";
		if (_channels[params[0]]->getChannelFlagIsSet(CHANNEL_MODES_INVITE_ONLY))
			modes_to_print += "i";
		if (_channels[params[0]]->getChannelFlagIsSet(CHANNEL_MODES_MODERATED))
			modes_to_print += "m";
		if (_channels[params[0]]->getChannelFlagIsSet(CHANNEL_MODES_NO_OUTSIDE))
			modes_to_print += "n";
		if (_channels[params[0]]->getChannelFlagIsSet(CHANNEL_MODES_QUIET))
			modes_to_print += "v";
		if (_channels[params[0]]->getChannelFlagIsSet(CHANNEL_MODES_PRIVATE))
			modes_to_print += "p";
		if (_channels[params[0]]->getChannelFlagIsSet(CHANNEL_MODES_SECRET))
			modes_to_print += "s";
		if (_channels[params[0]]->getChannelFlagIsSet(CHANNEL_MODES_REOP))
			modes_to_print += "r";
		if (_channels[params[0]]->getChannelFlagIsSet(CHANNEL_MODES_TOPIC_OPERATOR))
			modes_to_print += "t";
		if (_channels[params[0]]->getIsPassword()) {
			modes_to_print += "k";
			mode_params += _channels[params[0]]->getChannelPassword();
			mode_params += " ";
		}
		if (_channels[params[0]]->getUserLimit() != 0) {
			modes_to_print += "l";
			mode_params += std::to_string(_channels[params[0]]->getUserLimit());
		}
		sendMessage(client, RPL_CHANNELMODEIS(params[0], modes_to_print, mode_params));
		return ;
	}

	if (params.size() < 2) {
		sendMessage(client, ERR_NEEDMOREPARAMS(std::string("MODE")));
		return ;
	}
	if (!_channels[params[0]]->getClientIsOperator(client->getNickname())) {
		sendMessage(client, ERR_CHANOPRIVSNEEDED(params[0]));
		return ;
	}

	std::string possible_flags = "aimnqpsrtoklbIe";
	std::string actual_flags = "+";
	std::string mode_comments;
	int n_modes_with_comments = 0;
	for (std::vector <std::string>::iterator i = params.begin(); i != params.end(); ++i) {
		if (*i == "+b" || *i == "-b" || *i == "+e" || *i == "-e" || *i == "+I" || *i == "-I")
			n_modes_with_comments++;

		if (*i == "+l" || *i == "-k" || *i == "+k")
			n_modes_with_comments++;
		if (*i == "+o" || *i == "-o")
			n_modes_with_comments++;
		if (n_modes_with_comments > 3) {
			sendMessage(client, ERR_UNKNOWNMODE(*i, params[0]));
			return ;
		}
	}

	for (size_t i = 1; i < params.size(); i++) {
		char sign = params[i][0];
		if (sign != '+' && sign != '-') {
			sendMessage(client, ERR_UNKNOWNMODE(params[i], params[0]));
			return ;
		}
		std::string flag_to_use = params[i].substr(1);
		if (possible_flags.find(flag_to_use) == std::string::npos) {
			sendMessage(client, ERR_UNKNOWNMODE(params[i], params[0]));
			return ;
		}

		if (flag_to_use == "b" && sign == '+') {
			i++;
			if (i >= params.size()) {
				for (std::set <std::string>::iterator i = _channels[params[0]]->getBanmask().begin(); i != _channels[params[0]]->getBanmask().end(); ++i) {
					sendMessage(client, RPL_BANLIST(params[0], *i));
				}
				sendMessage(client, RPL_ENDOFBANLIST(params[0]));
				continue;
			}
			mode_comments = params[i];
			_channels[params[0]]->setBanMask(mode_comments);
			actual_flags += "b ";
			for (std::set <std::string>::iterator i = _channels[params[0]]->getBanmask().begin(); i != _channels[params[0]]->getBanmask().end(); ++i) {
				sendMessage(client, RPL_BANLIST(params[0], *i));
			}
			sendMessage(client, RPL_ENDOFBANLIST(params[0]));
			continue;
		}
		if (flag_to_use == "b" && sign == '-') {
			i++;
			
			if (i >= params.size()) {
				continue;
			}
			_channels[params[0]]->unsetBanmask(params[i]);
			for (std::set <std::string>::iterator i = _channels[params[0]]->getBanmask().begin(); i != _channels[params[0]]->getBanmask().end(); ++i) {
				sendMessage(client, RPL_BANLIST(params[0], *i));
			}
			sendMessage(client, RPL_ENDOFBANLIST(params[0]));
			if (actual_flags.find("b") != std::string::npos) {
				actual_flags.erase(actual_flags.find("b "), 2);
			}
			continue;
		}

		if (flag_to_use == "e" && sign == '+') {
			
			i++;
			if (i >= params.size()) {
				for (std::set <std::string>::iterator i = _channels[params[0]]->getExceptionmask().begin(); i != _channels[params[0]]->getExceptionmask().end(); ++i) {
					sendMessage(client, RPL_EXCEPTLIST(params[0], *i));
				}
				sendMessage(client, RPL_ENDOFEXCEPTLIST(params[0]));
				continue;
			}
			mode_comments = params[i];
			_channels[params[0]]->setExceptionMask(mode_comments);
			actual_flags += "e ";
			for (std::set <std::string>::iterator i = _channels[params[0]]->getExceptionmask().begin(); i != _channels[params[0]]->getExceptionmask().end(); ++i) {
				sendMessage(client, RPL_EXCEPTLIST(params[0], *i));
			}
			sendMessage(client, RPL_ENDOFEXCEPTLIST(params[0]));
			continue;
		}
		if (flag_to_use == "e" && sign == '-') {
			if (i >= params.size()) {
				continue;
			}
			_channels[params[0]]->unsetExceptionmask(params[i]);
			for (std::set <std::string>::iterator i = _channels[params[0]]->getExceptionmask().begin(); i != _channels[params[0]]->getExceptionmask().end(); ++i) {
				sendMessage(client, RPL_EXCEPTLIST(params[0], *i));
			}
			if (actual_flags.find("e") != std::string::npos) {
				actual_flags.erase(actual_flags.find("e "), 2);
			}
			sendMessage(client, RPL_ENDOFEXCEPTLIST(params[0]));
			continue;
		}

		if (flag_to_use == "I" && sign == '+') {
			i++;
			if (i >= params.size()) {
				for (std::set <std::string>::iterator i = _channels[params[0]]->getInvitemask().begin(); i != _channels[params[0]]->getInvitemask().end(); ++i) {
					sendMessage(client, RPL_INVITELIST(params[0], *i));
				}
				sendMessage(client, RPL_ENDOFINVITELIST(params[0]));
				continue;
			}
			mode_comments = params[i];
			_channels[params[0]]->setInviteMask(mode_comments);
			actual_flags += "I ";
			for (std::set <std::string>::iterator i = _channels[params[0]]->getInvitemask().begin(); i != _channels[params[0]]->getInvitemask().end(); ++i) {
				sendMessage(client, RPL_INVITELIST(params[0], *i));
			}
			sendMessage(client, RPL_ENDOFINVITELIST(params[0]));
			continue;
		}
		if (flag_to_use == "I" && sign == '-') {
			if (i >= params.size()) {
				continue;
			}
			_channels[params[0]]->unsetInvitemask(params[i]);
			for (std::set <std::string>::iterator i = _channels[params[0]]->getInvitemask().begin(); i != _channels[params[0]]->getInvitemask().end(); ++i) {
				sendMessage(client, RPL_INVITELIST(params[0], *i));
			}
			if (actual_flags.find("I") != std::string::npos) {
				actual_flags.erase(actual_flags.find("I "), 2);
			}

			sendMessage(client, RPL_ENDOFINVITELIST(params[0]));
			continue;
		}

		if (flag_to_use == "o" && sign == '+') {
			i++;
			if (i >= params.size()) {
				sendMessage(client, ERR_NEEDMOREPARAMS(std::string("MODE")));
				return ;
			}
			if (!_channels[params[0]]->getClientIsConnected(params[i])) {
				sendMessage(client, ERR_USERNOTINCHANNEL(params[i], params[0]));
				return ;
			}
			mode_comments = params[i];
			_channels[params[0]]->setClientOperator(mode_comments);
			actual_flags += "o ";
			sendMessage(client,RPL_CHANNELMODEIS(params[0], "+o", mode_comments)); 
			continue;
		}
		if (flag_to_use == "o" && sign == '-') {
			i++;
			if (i >= params.size()) {
				sendMessage(client, ERR_NEEDMOREPARAMS(std::string("MODE")));
				return ;
			}
			if (!_channels[params[0]]->getClientIsConnected(params[i])) {
				sendMessage(client, ERR_USERNOTINCHANNEL(params[i], params[0]));
				return ;
			}
			mode_comments = params[i];
			_channels[params[0]]->unsetClientOperator(mode_comments);
			if (actual_flags.find("o") != std::string::npos) {
				actual_flags.erase(actual_flags.find("o "), 2);
			}
			sendMessage(client, RPL_CHANNELMODEIS(params[0], "-o", mode_comments)); 
			continue;
		}

		if (flag_to_use == "k" || flag_to_use == "l") {
			i++;
			if (i >= params.size() && !(sign == '-' && flag_to_use == "l")) {
				sendMessage(client, ERR_NEEDMOREPARAMS(std::string("MODE")));
				return ;
			}
			if (flag_to_use == "l" && sign == '-') {
				_channels[params[0]]->setUserLimit(0);
				if (actual_flags.find("l") != std::string::npos) {
					actual_flags.erase(actual_flags.find("l "), 2);
				}
				continue;
			}

			mode_comments = params[i];
			if (flag_to_use == "k") {
				if (mode_comments != _channels[params[0]]->getChannelPassword() && sign == '-') {
					sendMessage(client, ERR_INVALIDKEY(client->getNickname(), params[0]));
					return ;
				}
				if (mode_comments == _channels[params[0]]->getChannelPassword() && sign == '-') {
					_channels[params[0]]->setPassword("");
					if (actual_flags.find("k") != std::string::npos) {
						actual_flags.erase(actual_flags.find("k "), 2);
					}
					continue;
				}
				if (_channels[params[0]]->getIsPassword() && sign == '+') {
					sendMessage(client, ERR_KEYSET(params[0]));
					return ;
				}
				_channels[params[0]]->setPassword(mode_comments);
				actual_flags += "k ";
				continue;
			}
			if (sign == '-') {
				_channels[params[0]]->setUserLimit(0);
				continue;
			}
			_channels[params[0]]->setUserLimit(stoi(mode_comments));
			actual_flags += "l ";
			continue;
		}


		int flag_to_use_num;
		if (flag_to_use == "a")
			flag_to_use_num = CHANNEL_MODES_ANONYMOUS;
		else if (flag_to_use == "i")
			flag_to_use_num = CHANNEL_MODES_INVITE_ONLY;
		else if (flag_to_use == "m")
			flag_to_use_num = CHANNEL_MODES_MODERATED;
		else if (flag_to_use == "n")
			flag_to_use_num = CHANNEL_MODES_NO_OUTSIDE;
		else if (flag_to_use == "q")
			flag_to_use_num = CHANNEL_MODES_QUIET;
		else if (flag_to_use == "p")
			flag_to_use_num = CHANNEL_MODES_PRIVATE;
		else if (flag_to_use == "s")
			flag_to_use_num = CHANNEL_MODES_SECRET;
		else if (flag_to_use == "r")
			flag_to_use_num = CHANNEL_MODES_REOP;
		else if (flag_to_use == "t")
			flag_to_use_num = CHANNEL_MODES_TOPIC_OPERATOR;
		else {
			sendMessage(client, ERR_UNKNOWNMODE(params[i], params[0]));
				return ;
		}

		if (sign == '+' && !_channels[params[0]]->getChannelFlagIsSet(flag_to_use_num)) {
			_channels[params[0]]->toggleFlag(flag_to_use_num);
			actual_flags += flag_to_use;
			actual_flags += " ";
		}
		if (sign == '-' && _channels[params[0]]->getChannelFlagIsSet(flag_to_use_num)) {
			_channels[params[0]]->toggleFlag(flag_to_use_num);
		}
	}
	actual_flags += "\r\n";
	sendMessage(client, RPL_CHANNELMODEIS(params[0], modes_to_print, mode_comments));
}
