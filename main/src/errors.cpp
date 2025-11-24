/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kcisse <kcisse@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/22 01:17:45 by marvin            #+#    #+#             */
/*   Updated: 2025/11/24 19:01:27 by kcisse           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Irc.hpp>
#include <sstream>

std::string Errors::welcome_banner() {
	std::ostringstream oss;
	oss << "\r\n"
		<< "  ╔═══════════════════════════════════════╗\r\n"
		<< "  ║     Welcome to Sockell IRC Server     ║\r\n"
		<< "  ╠═══════════════════════════════════════╣\r\n"
		<< "  ║                                       ║\r\n"
		<< "  ║  1. PASS <password>                   ║\r\n"
		<< "  ║  2. NICK <nickname>                   ║\r\n"
		<< "  ║  3. USER <username>                   ║\r\n"
		<< "  ║                                       ║\r\n"
		<< "  ║  Then: JOIN #general                  ║\r\n"
		<< "  ║                                       ║\r\n"
		<< "  ╚═══════════════════════════════════════╝\r\n"
		<< "\r\n";
	return oss.str();
}

std::string Errors::password_prompt() {
	return "Please authenticate with: PASS <password>\r\n";
}

std::string Errors::authenticated() {
	return "✓ Authentication successful! Use NICK and USER to register.\r\n";
}

std::string Errors::registered(const std::string &nick, const std::string &user, const std::string &host) {
	std::ostringstream oss;
	oss << "\r\n"
		<< "  ╔═══════════════════════════════════════╗\r\n"
		<< "  ║  ✓ Registration complete!             ║\r\n"
		<< "  ╠═══════════════════════════════════════╣\r\n"
		<< "  ║                                       ║\r\n"
		<< "  ║  You are: " << nick << "!" << user << "@" << host << "\r\n"
		<< "  ║                                       ║\r\n"
		<< "  ║  Available channels:                  ║\r\n"
		<< "  ║    • #general - Main discussion       ║\r\n"
		<< "  ║    • #help    - Commands reference    ║\r\n"
		<< "  ║                                       ║\r\n"
		<< "  ║  Quick start:                         ║\r\n"
		<< "  ║    JOIN #general                      ║\r\n"
		<< "  ║    PRIVMSG #general :Hello!           ║\r\n"
		<< "  ║                                       ║\r\n"
		<< "  ╚═══════════════════════════════════════╝\r\n"
		<< "\r\n";
	return oss.str();
}

std::string Errors::wrong_password(int attempts) {
	std::ostringstream oss;
	oss << "✗ Wrong password (" << attempts << "/3)\r\n";
	return oss.str();
}

std::string Errors::too_many_attempts() {
	return "✗ Too many failed attempts. Disconnecting...\r\n";
}

std::string Errors::nickname_changed(const std::string &old_nick, const std::string &new_nick) {
	std::ostringstream oss;
	oss << "✓ Nickname changed: " << old_nick << " → " << new_nick << "\r\n";
	return oss.str();
}

std::string Errors::nickname_in_use(const std::string &nick) {
	std::ostringstream oss;
	oss << "✗ Nickname '" << nick << "' is already in use\r\n";
	return oss.str();
}

std::string Errors::joined_channel(const std::string &channel) {
	std::ostringstream oss;
	oss << "✓ Joined #" << channel << " - Use PRIVMSG #" << channel
		<< " :text to send messages\r\n";
	return oss.str();
}

std::string Errors::parted_channel(const std::string &channel, const std::string &reason) {
	std::ostringstream oss;
	oss << "✓ Left #" << channel;
	if (!reason.empty())
		oss << " (" << reason << ")";
	oss << "\r\n";
	return oss.str();
}

std::string Errors::help_commands() {
	std::ostringstream oss;
	oss << "\r\n"
		<< "╔═══════════════════════════════════════════════════╗\r\n"
		<< "║              SOCKELL IRC COMMANDS\r\n"
		<< "╠═══════════════════════════════════════════════════╣\r\n"
		<< "║\r\n"
		<< "║  CHANNELS\r\n"
		<< "║    JOIN #channel [key]    Join a channel\r\n"
		<< "║    PART #channel          Leave a channel\r\n"
		<< "║    TOPIC #channel [text]  View/set topic\r\n"
		<< "║\r\n"
		<< "║  MESSAGES\r\n"
		<< "║    PRIVMSG #channel :text Send to channel\r\n"
		<< "║    PRIVMSG nick :text     Private message\r\n"
		<< "║\r\n"
		<< "║  OPERATORS (requires @)\r\n"
		<< "║    KICK #chan nick [:reason]\r\n"
		<< "║    INVITE nick #channel\r\n"
		<< "║    MODE #chan +i          Invite-only\r\n"
		<< "║    MODE #chan +t          Protected topic\r\n"
		<< "║    MODE #chan +k <key>    Set password\r\n"
		<< "║    MODE #chan +o <nick>   Give operator\r\n"
		<< "║    MODE #chan +l <num>    Set user limit\r\n"
		<< "║\r\n"
		<< "║  MISC\r\n"
		<< "║    QUIT [:message]        Disconnect\r\n"
		<< "║    PING <token>           Test connection\r\n"
		<< "║\r\n"
		<< "╚═══════════════════════════════════════════════════╝\r\n"
		<< "\r\n"
		<< "Note: Messages must start with PRIVMSG!\r\n"
		<< "Example: PRIVMSG #general :Hello everyone!\r\n"
		<< "\r\n";
	return oss.str();
}

std::string Errors::cannot_send_to_help() {
	return "✗ #help is read-only. Use JOIN #help to see commands.\r\n";
}

std::string Errors::unknown_command(const std::string &cmd) {
	std::ostringstream oss;
	oss << "✗ Unknown command: " << cmd << "\r\n";
	return oss.str();
}

std::string Errors::not_authenticated() {
	return "✗ Please authenticate first: PASS <password>\r\n";
}
