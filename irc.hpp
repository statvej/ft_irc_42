#ifndef IRC_HPP
# define IRC_HPP

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <vector>
#include <string>
#include <unistd.h>
#include <map>
#include <utility>
#include <sys/select.h>
#include <sys/time.h>
#include <fcntl.h>
#include <iostream>
#include <stdlib.h>
#include <sstream>
#include <csignal>

#include "Replies.hpp"

#define MESSAGE_BUFFER_SIZE 512
#define HOST_NAME "better_than_slack"
#define BOT_NAME "chatgpt"
#define MOTD "Every book is just a Dictionary remix"

bool strIsASCII(const std::string& str);
std::string removeNonPrintableCharacters(const std::string& str);

std::string getNickFromPrefix(std::string prefix);

#endif
