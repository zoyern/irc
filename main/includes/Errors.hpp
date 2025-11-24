#pragma once
#include <string>

struct Errors
{
	static std::string welcome_banner();
	static std::string password_prompt();
	static std::string authenticated();
	static std::string registered(const std::string &nick, const std::string &user, const std::string &host);
	static std::string wrong_password(int attempts);
	static std::string too_many_attempts();
	static std::string nickname_changed(const std::string &old_nick, const std::string &new_nick);
	static std::string nickname_in_use(const std::string &nick);
	static std::string joined_channel(const std::string &channel);
	static std::string parted_channel(const std::string &channel, const std::string &reason);
	static std::string help_commands();
	static std::string cannot_send_to_help();
	static std::string unknown_command(const std::string &cmd);
	static std::string not_authenticated();
};
