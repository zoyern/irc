#include "Irc.hpp"

void on_signal(void *event, void *user_data) {
	SkllEvent *evt = (SkllEvent*)event; (void)user_data;
	std::cout << SKLL_MAGENTA << "\n[" << irc_timestamp() << "] "
			  << SKLL_RESET << "Signal " << SKLL_BOLD << evt->signal_num
			  << SKLL_RESET << " - Graceful shutdown" << std::endl;
}

std::string irc_timestamp() {
	struct timeval tv;
	gettimeofday(&tv, NULL);

	std::time_t now = tv.tv_sec;
	std::tm *tm_info = std::localtime(&now);
	int ms = tv.tv_usec / 1000;

	std::ostringstream oss;
	oss << std::setfill('0')
		<< std::setw(2) << tm_info->tm_hour << ":"
		<< std::setw(2) << tm_info->tm_min << ":"
		<< std::setw(2) << tm_info->tm_sec << "."
		<< std::setw(3) << ms;

	return oss.str();
}

// std::string irc_user_info(Client *client) {
// 	std::ostringstream oss;
// 	oss << std::setw(12) << std::left << (client->username.empty() ? "*" : client->username)
// 		<< ":" << std::setw(12) << std::left << (client->nickname.empty() ? "*" : client->nickname)
// 		<< " (" << client->get_id() << ")";
// 	return oss.str();
// }

// void irc_log(const std::string &event, IRCClient *client, const std::string &message) {
// 	std::cout << "[" << irc_timestamp() << "]\t"
// 			  << std::setw(35) << std::left << irc_user_info(client) << "\t"
// 			  << "[" << std::setw(10) << std::left << event << "]\t-> "
// 			  << message << std::endl;
// }

void irc_log_simple(const std::string &event, const std::string &message) {
	std::cout << "[" << irc_timestamp() << "]\t"
			  << std::setw(35) << std::left << "SERVER" << "\t"
			  << "[" << std::setw(10) << std::left << event << "]\t-> "
			  << message << std::endl;
}

void irc_log_command(const std::string &cmd, const std::string &details) {
	std::cout << "[" << irc_timestamp() << "]\t"
			  << std::setw(35) << std::left << "COMMAND" << "\t"
			  << "[" << std::setw(10) << std::left << cmd << "]\t-> "
			  << details << std::endl;
}

std::string irc_err_nosuchnick(const std::string &nick, const std::string &target) {
	return ":server " ERR_NOSUCHNICK " " + nick + " " + target + " :No such nick/channel\r\n";
}

std::string irc_err_nosuchchannel(const std::string &nick, const std::string &channel) {
	return ":server " ERR_NOSUCHCHANNEL " " + nick + " " + channel + " :No such channel\r\n";
}

std::string irc_err_cannotsendtochan(const std::string &nick, const std::string &channel) {
	return ":server " ERR_CANNOTSENDTOCHAN " " + nick + " " + channel + " :Cannot send to channel\r\n";
}

std::string irc_err_nonicknamegiven() {
	return ":server " ERR_NONICKNAMEGIVEN " * :No nickname given\r\n";
}

std::string irc_err_nicknameinuse(const std::string &nick) {
	return ":server " ERR_NICKNAMEINUSE " * " + nick + " :Nickname is already in use\r\n";
}

std::string irc_err_notonchannel(const std::string &nick, const std::string &channel) {
	return ":server " ERR_NOTONCHANNEL " " + nick + " #" + channel + " :Not on channel\r\n";
}

std::string irc_err_notregistered(const std::string &nick) {
	std::string n = nick.empty() ? "*" : nick;
	return ":server " ERR_NOTREGISTERED " " + n + " :You have not registered\r\n";
}

std::string irc_err_needmoreparams(const std::string &nick, const std::string &command) {
	return ":server " ERR_NEEDMOREPARAMS " " + nick + " " + command + " :Not enough parameters\r\n";
}

std::string irc_err_alreadyregistred(const std::string &nick) {
	return ":server " ERR_ALREADYREGISTRED " " + nick + " :You may not reregister\r\n";
}

std::string irc_err_channelisfull(const std::string &nick, const std::string &channel) {
	return ":server " ERR_CHANNELISFULL " " + nick + " #" + channel + " :Channel is full\r\n";
}

std::string irc_err_inviteonlychan(const std::string &nick, const std::string &channel) {
	return ":server " ERR_INVITEONLYCHAN " " + nick + " #" + channel + " :Invite only\r\n";
}

std::string irc_err_badchannelkey(const std::string &nick, const std::string &channel) {
	return ":server " ERR_BADCHANNELKEY " " + nick + " #" + channel + " :Bad channel key\r\n";
}

std::string irc_err_chanoprivsneeded(const std::string &nick, const std::string &channel) {
	return ":server " ERR_CHANOPRIVSNEEDED " " + nick + " #" + channel + " :You're not channel operator\r\n";
}

std::string irc_err_notexttosend(const std::string &nick) {
	return ":server 412 " + nick + " :No text to send\r\n";
}

std::string irc_rpl_welcome(const std::string &nick, const std::string &user, const std::string &host, const std::string &servername) {
	return ":server " RPL_WELCOME " " + nick + " :Welcome to " + servername + " " + nick + "!" + user + "@" + host + "\r\n";
}

std::string irc_rpl_topic(const std::string &nick, const std::string &channel, const std::string &topic) {
	return ":server " RPL_TOPIC " " + nick + " #" + channel + " :" + topic + "\r\n";
}
