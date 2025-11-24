#pragma once

#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <thread>
#include <vector>
#include <sstream>
#include <sys/time.h>
#include <iomanip>

#include <Sockall.hpp>
#include "Client.hpp"
#include "Channel.hpp"
#include "Server.hpp"
#include "Cmds.hpp"

#define IRC_CRLF    (1 << 0)
#define IRC_LF      (1 << 1)

#define IRC_COLOR_SERVER    SKLL_CYAN
#define IRC_COLOR_JOIN      SKLL_GREEN
#define IRC_COLOR_PART      SKLL_YELLOW
#define IRC_COLOR_QUIT      SKLL_RED
#define IRC_COLOR_PRIVMSG   SKLL_WHITE
#define IRC_COLOR_MODE      SKLL_MAGENTA
#define IRC_COLOR_ERROR     SKLL_RED SKLL_BOLD

#define RPL_WELCOME             "001"
#define RPL_YOURHOST            "002"
#define RPL_CREATED             "003"
#define RPL_MYINFO              "004"
#define RPL_NOTOPIC             "331"
#define RPL_TOPIC               "332"
#define RPL_NAMREPLY            "353"
#define RPL_ENDOFNAMES          "366"
#define RPL_MOTD                "372"
#define RPL_MOTDSTART           "375"
#define RPL_ENDOFMOTD           "376"

#define ERR_NOSUCHNICK          "401"
#define ERR_NOSUCHCHANNEL       "403"
#define ERR_CANNOTSENDTOCHAN    "404"
#define ERR_TOOMANYCHANNELS     "405"
#define ERR_UNKNOWNCOMMAND      "421"
#define ERR_NONICKNAMEGIVEN     "431"
#define ERR_ERRONEUSNICKNAME    "432"
#define ERR_NICKNAMEINUSE       "433"
#define ERR_USERNOTINCHANNEL    "441"
#define ERR_NOTONCHANNEL        "442"
#define ERR_USERONCHANNEL       "443"
#define ERR_NOTREGISTERED       "451"
#define ERR_NEEDMOREPARAMS      "461"
#define ERR_ALREADYREGISTRED    "462"
#define ERR_PASSWDMISMATCH      "464"
#define ERR_CHANNELISFULL       "471"
#define ERR_INVITEONLYCHAN      "473"
#define ERR_BADCHANNELKEY       "475"
#define ERR_CHANOPRIVSNEEDED    "482"

std::string irc_timestamp();
// std::string irc_user_info(IRCClient *client);
// void        irc_log(const std::string &event, IRCClient *client, const std::string &message);
void        irc_log_simple(const std::string &event, const std::string &message);
void        irc_log_command(const std::string &cmd, const std::string &details);


std::string irc_err_nosuchnick(const std::string &nick, const std::string &target);
std::string irc_err_nosuchchannel(const std::string &nick, const std::string &channel);
std::string irc_err_cannotsendtochan(const std::string &nick, const std::string &channel);
std::string irc_err_nonicknamegiven();
std::string irc_err_nicknameinuse(const std::string &nick);
std::string irc_err_notonchannel(const std::string &nick, const std::string &channel);
std::string irc_err_notregistered(const std::string &nick);
std::string irc_err_needmoreparams(const std::string &nick, const std::string &command);
std::string irc_err_alreadyregistred(const std::string &nick);
std::string irc_err_channelisfull(const std::string &nick, const std::string &channel);
std::string irc_err_inviteonlychan(const std::string &nick, const std::string &channel);
std::string irc_err_badchannelkey(const std::string &nick, const std::string &channel);
std::string irc_err_chanoprivsneeded(const std::string &nick, const std::string &channel);
std::string irc_err_notexttosend(const std::string &nick);

std::string irc_rpl_welcome(const std::string &nick, const std::string &user, const std::string &host, const std::string &servername);
std::string irc_rpl_topic(const std::string &nick, const std::string &channel, const std::string &topic);
void on_signal(void *event, void *user_data);
