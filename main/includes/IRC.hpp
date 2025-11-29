
#pragma once

#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <vector>
#include <map>
#include <sys/types.h>
#include <fcntl.h>
#include <poll.h>
#include <csignal>
#include <sstream>

#define RESET		"\033[0m"
#define BOLD		"\033[1m"
#define DIM		"\033[2m"
#define RED		"\033[91m"
#define GREEN		"\033[92m"
#define YELLOW		"\033[93m"
#define BLUE		"\033[94m"
#define MAGENTA	"\033[95m"
#define CYAN		"\033[96m"
#define WHITE		"\033[97m"

typedef unsigned char	byte_t;
typedef int				fd_t;

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   IRC NUMERIC REPLIES - RFC 2812                                            */
/* ═══════════════════════════════════════════════════════════════════════════ */

/* Connection Registration */
# define RPL_WELCOME           "001"
# define RPL_YOURHOST          "002"
# define RPL_CREATED           "003"
# define RPL_MYINFO            "004"

/* Command Replies */
# define RPL_UMODEIS           "221"
# define RPL_CHANNELMODEIS     "324"
# define RPL_NOTOPIC           "331"
# define RPL_TOPIC             "332"
# define RPL_INVITING          "341"
# define RPL_NAMREPLY          "353"
# define RPL_ENDOFNAMES        "366"
# define RPL_WHOREPLY          "352"
# define RPL_ENDOFWHO          "315"

/* Error Replies */
# define ERR_NOSUCHNICK        "401"
# define ERR_NOSUCHCHANNEL     "403"
# define ERR_CANNOTSENDTOCHAN  "404"
# define ERR_NORECIPIENT       "411"
# define ERR_NOTEXTTOSEND      "412"
# define ERR_UNKNOWNCOMMAND    "421"
# define ERR_NONICKNAMEGIVEN   "431"
# define ERR_ERRONEUSNICKNAME  "432"
# define ERR_NICKNAMEINUSE     "433"
# define ERR_USERNOTINCHANNEL  "441"
# define ERR_NOTONCHANNEL      "442"
# define ERR_USERONCHANNEL     "443"
# define ERR_NOTREGISTERED     "451"
# define ERR_NEEDMOREPARAMS    "461"
# define ERR_ALREADYREGISTRED  "462"
# define ERR_PASSWDMISMATCH    "464"
# define ERR_CHANNELISFULL     "471"
# define ERR_UNKNOWNMODE       "472"
# define ERR_INVITEONLYCHAN    "473"
# define ERR_BADCHANNELKEY     "475"
# define ERR_CHANOPRIVSNEEDED  "482"

#include <IRC/IRCUser.hpp>
#include <IRC/IRCChannel.hpp>
#include <IRC/IRCServer.hpp>

bool checkPort(std::string strPort);
std::vector<std::string> splitCmdLine(std::string str);
std::vector<std::string> splitCmd(std::string input);