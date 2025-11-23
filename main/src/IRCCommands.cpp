/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCCommands.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/23 19:13:01 by marvin            #+#    #+#             */
/*   Updated: 2025/11/23 19:13:01 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <IRC/IRCCommands.hpp>
#include <IRC/IRCServer.hpp>
#include <IRC/IRCClient.hpp>
#include <IRC/IRCChannel.hpp>
#include <IRC/IRCErrors.hpp>
#include <IRC.hpp>
#include <Sockell.hpp>
#include <unistd.h>
#include <sstream>

void irc_cmd_pass(SkllEvent *event, void *user_data) {
	IRCServer *server = (IRCServer*)user_data;
	IRCClient *client = (IRCClient*)event->client->userdata;
	
	if (!client || client->is_registered()) return;
	
	std::string password = event->message->to<std::string>();
	
	if (server->check_password(password)) {
		client->authenticated = true;
		client->send(IRCErrors::authenticated());
		irc_log("AUTH", client, "authenticated successfully");
	} else {
		client->password_attempts++;
		if (client->password_attempts >= 3) {
			client->send(IRCErrors::too_many_attempts());
			irc_log("AUTH", client, "3 failed attempts - disconnecting");
			close(client->get_fd());
		} else {
			client->send(IRCErrors::wrong_password(client->password_attempts));
			std::ostringstream oss;
			oss << "wrong password (" << client->password_attempts << "/3)";
			irc_log("AUTH", client, oss.str());
		}
	}
}

void irc_cmd_nick(SkllEvent *event, void *user_data) {
	IRCServer *server = (IRCServer*)user_data;
	IRCClient *client = (IRCClient*)event->client->userdata;
	
	if (!client) return;
	
	if (!client->is_authenticated()) {
		client->send(IRCErrors::not_authenticated());
		return;
	}
	
	std::string new_nick = event->message->to<std::string>();
	
	if (new_nick.empty()) {
		client->send(irc_err_nonicknamegiven());
		return;
	}
	
	if (server->get_client_by_nick(new_nick)) {
		client->send(IRCErrors::nickname_in_use(new_nick));
		client->send(irc_err_nicknameinuse(new_nick));
		return;
	}
	
	std::string old_nick = client->nickname;
	client->nickname = new_nick;

	std::set<std::string>::iterator it;
	for (it = client->channels.begin(); it != client->channels.end(); ++it) {
		IRCChannel *channel = server->get_channel(*it);
		if (channel) {
			std::ostringstream oss;
			oss << ":" << (old_nick.empty() ? "*" : old_nick) << "!" 
				<< client->username << "@" << client->hostname
				<< " NICK :" << new_nick << "\r\n";
			channel->broadcast(oss.str());
		}
	}

	std::ostringstream oss;
	oss << ":" << (old_nick.empty() ? "*" : old_nick) 
		<< " NICK :" << new_nick << "\r\n";
	client->send(oss.str());

	if (!old_nick.empty()) {
		irc_log("NICK", client, "from (" + old_nick + ") to -> (" + new_nick + ")");
	} else {
		irc_log("NICK", client, "set to (" + new_nick + ")");
	}
}

void irc_cmd_user(SkllEvent *event, void *user_data) {
	IRCServer *server = (IRCServer*)user_data;
	IRCClient *client = (IRCClient*)event->client->userdata;
	
	if (!client) return;
	
	if (!client->is_authenticated()) {
		client->send(IRCErrors::not_authenticated());
		client->send(irc_err_notregistered("*"));
		return;
	}
	
	if (client->is_registered()) 
		return (client->send(irc_err_alreadyregistred(client->nickname)));
	client->username = event->message->to<std::string>();
	event->message->to<std::string>();
	event->message->to<std::string>();
	client->realname = event->message->to();
	
	if (!client->nickname.empty() && !client->username.empty()) {
		client->registered = true;
		client->send(irc_rpl_welcome(client->nickname, client->username, client->hostname, server->get_name()));
		client->send(IRCErrors::registered(client->nickname, client->username, client->hostname));
		irc_log("REGISTER", client, "complete");
	}
}

void irc_cmd_join(SkllEvent *event, void *user_data) {
	IRCServer *server = (IRCServer*)user_data;
	IRCClient *client = (IRCClient*)event->client->userdata;
	if (!client) return;
	if (!client->is_registered())
		return (client->send(irc_err_notregistered(client->nickname)));
	std::string chan_name = event->message->to<std::string>();
	std::string key;
	if (event->message->has_more())
		key = event->message->to<std::string>();
	if (!chan_name.empty() && chan_name[0] == '#')
		chan_name = chan_name.substr(1);
	if (client->is_in_channel(chan_name))
		return;
	IRCChannel *channel = server->create_channel(chan_name);
	if (!channel->can_join(client->get_fd(), key)) {
		if (channel->is_full()) {
			client->send(irc_err_channelisfull(client->nickname, chan_name));
		} else if (channel->invite_only) {
			client->send(irc_err_inviteonlychan(client->nickname, chan_name));
		} else
			client->send(irc_err_badchannelkey(client->nickname, chan_name));
		return;
	}
	bool is_first = (channel->get_client_count() == 0);
	if (is_first)
		channel->set_operator(client->get_fd(), true);
	channel->skll_channel->add_client(client->get_fd(), client->skll_client);
	client->join_channel(chan_name);
	std::ostringstream join_msg;
	join_msg << ":" << client->nickname << "!" << client->username << "@" << client->hostname
			 << " JOIN :#" << chan_name << "\r\n";
	channel->broadcast(join_msg.str());
	if (!channel->topic.empty()) 
		client->send(irc_rpl_topic(client->nickname, chan_name, channel->topic));
	client->send(IRCErrors::joined_channel(chan_name));
	if (chan_name == "help")
		client->send(IRCErrors::help_commands());
	std::ostringstream log_msg;
	log_msg << "#" << chan_name << " [" << channel->get_client_count() << " users]";
	if (is_first) log_msg << " (creator @)";
	irc_log("JOIN", client, log_msg.str());
}

void irc_cmd_part(SkllEvent *event, void *user_data) {
	IRCServer *server = (IRCServer*)user_data;
	IRCClient *client = (IRCClient*)event->client->userdata;
	
	if (!client) return;
	
	std::string chan_name = event->message->to<std::string>();
	std::string reason;
	
	if (event->message->has_more()) {
		reason = event->message->to();
	}
	
	if (!chan_name.empty() && chan_name[0] == '#') {
		chan_name = chan_name.substr(1);
	}
	
	IRCChannel *channel = server->get_channel(chan_name);
	
	if (!channel) {
		client->send(irc_err_nosuchchannel(client->nickname, chan_name));
		return;
	}
	
	if (!client->is_in_channel(chan_name)) {
		client->send(irc_err_notonchannel(client->nickname, chan_name));
		return;
	}
	
	std::ostringstream part_msg;
	part_msg << ":" << client->nickname << "!" << client->username << "@" << client->hostname
			 << " PART #" << chan_name;
	if (!reason.empty()) part_msg << " :" << reason;
	part_msg << "\r\n";
	channel->broadcast(part_msg.str());
	
	channel->skll_channel->remove_client(client->get_fd());
	client->part_channel(chan_name);
	
	client->send(IRCErrors::parted_channel(chan_name, reason));

	std::string log_msg = "#" + chan_name;
	if (!reason.empty()) log_msg += " (" + reason + ")";
	irc_log("PART", client, log_msg);
}

void irc_cmd_privmsg(SkllEvent *event, void *user_data) {
	IRCServer *server = (IRCServer*)user_data;
	IRCClient *client = (IRCClient*)event->client->userdata;
	
	if (!client || !client->is_registered()) return;
	
	std::string target = event->message->to<std::string>();
	
	if (!target.empty() && target[target.length() - 1] == ':') {
		target = target.substr(0, target.length() - 1);
	}
	
	std::string text = event->message->to();
	
	if (target.empty() || text.empty()) {
		client->send(irc_err_notexttosend(client->nickname));
		return;
	}
	
	if (target[0] == '#') {
		std::string chan_name = target.substr(1);
		IRCChannel *channel = server->get_channel(chan_name);
		
		if (!channel) {
			client->send(irc_err_nosuchchannel(client->nickname, target));
			return;
		}
		
		if (!client->is_in_channel(chan_name)) {
			client->send(irc_err_cannotsendtochan(client->nickname, target));
			return;
		}
		
		if (channel->read_only && !channel->is_operator(client->get_fd())) {
			client->send(IRCErrors::cannot_send_to_help());
			client->send(":server 404 " + client->nickname + " " + target + " :Cannot send to channel\r\n");
			return;
		}
		
		std::ostringstream msg;
		msg << ":" << client->nickname << "!" << client->username << "@" << client->hostname
			<< " PRIVMSG " << target << " :" << text << "\r\n";
		channel->broadcast(msg.str(), client->get_fd());

		std::string preview = text.length() > 50 ? text.substr(0, 47) + "..." : text;
		irc_log("MSG", client, target + " \"" + preview + "\"");
		
	} else {
		IRCClient *target_client = server->get_client_by_nick(target);
		
		if (!target_client) {
			client->send(irc_err_nosuchnick(client->nickname, target));
			return;
		}
		
		std::ostringstream msg;
		msg << ":" << client->nickname << "!" << client->username << "@" << client->hostname
			<< " PRIVMSG " << target << " :" << text << "\r\n";
		target_client->send(msg.str());

		std::string preview = text.length() > 50 ? text.substr(0, 47) + "..." : text;
		irc_log("PM", client, "to " + target + " \"" + preview + "\"");
	}
}

void irc_cmd_mode(SkllEvent *event, void *user_data) {
	(void)user_data;
	(void)event;
}

void irc_cmd_mode_i(SkllEvent *event, void *user_data) {
	IRCServer *server = (IRCServer*)user_data;
	IRCClient *client = (IRCClient*)event->client->userdata;
	
	if (!client) return;
	
	event->message->rewind();
	event->message->skip(5);
	std::string target = event->message->to<std::string>();
	
	if (target[0] == '#') {
		std::string chan_name = target.substr(1);
		IRCChannel *channel = server->get_channel(chan_name);
		
		if (channel && channel->is_operator(client->get_fd())) {
			channel->invite_only = !channel->invite_only;
			irc_log_command("MODE", target + " +i (invite-only)");
		}
	}
}

void irc_cmd_mode_t(SkllEvent *event, void *user_data) {
	IRCServer *server = (IRCServer*)user_data;
	IRCClient *client = (IRCClient*)event->client->userdata;
	
	if (!client) return;
	
	event->message->rewind();
	event->message->skip(5);
	std::string target = event->message->to<std::string>();
	
	if (target[0] == '#') {
		std::string chan_name = target.substr(1);
		IRCChannel *channel = server->get_channel(chan_name);
		
		if (channel && channel->is_operator(client->get_fd())) {
			channel->topic_protected = !channel->topic_protected;
			irc_log_command("MODE", target + " +t (topic protection)");
		}
	}
}

void irc_cmd_mode_k(SkllEvent *event, void *user_data) {
	IRCServer *server = (IRCServer*)user_data;
	IRCClient *client = (IRCClient*)event->client->userdata;
	
	if (!client) return;
	
	event->message->rewind();
	event->message->skip(5);
	std::string target = event->message->to<std::string>();
	std::string key = event->message->to<std::string>();
	
	if (target[0] == '#') {
		std::string chan_name = target.substr(1);
		IRCChannel *channel = server->get_channel(chan_name);
		
		if (channel && channel->is_operator(client->get_fd())) {
			channel->password = key;
			irc_log_command("MODE", target + " +k ***");
		}
	}
}

void irc_cmd_mode_o(SkllEvent *event, void *user_data) {
	IRCServer *server = (IRCServer*)user_data;
	IRCClient *client = (IRCClient*)event->client->userdata;
	
	if (!client) return;
	
	event->message->rewind();
	event->message->skip(5);
	std::string target = event->message->to<std::string>();
	std::string nick = event->message->to<std::string>();
	
	if (target[0] == '#') {
		std::string chan_name = target.substr(1);
		IRCChannel *channel = server->get_channel(chan_name);
		IRCClient *target_client = server->get_client_by_nick(nick);
		
		if (channel && target_client && channel->is_operator(client->get_fd())) {
			channel->set_operator(target_client->get_fd(), true);
			irc_log_command("MODE", target + " +o " + nick);
		}
	}
}

void irc_cmd_mode_l(SkllEvent *event, void *user_data) {
	IRCServer *server = (IRCServer*)user_data;
	IRCClient *client = (IRCClient*)event->client->userdata;
	
	if (!client) return;
	
	event->message->rewind();
	event->message->skip(5);
	std::string target = event->message->to<std::string>();
	int limit = event->message->to<int>();
	
	if (target[0] == '#') {
		std::string chan_name = target.substr(1);
		IRCChannel *channel = server->get_channel(chan_name);
		
		if (channel && channel->is_operator(client->get_fd())) {
			channel->user_limit = limit;
			std::ostringstream oss;
			oss << target << " +l " << limit;
			irc_log_command("MODE", oss.str());
		}
	}
}

void irc_cmd_kick(SkllEvent *event, void *user_data) {
	IRCServer *server = (IRCServer*)user_data;
	IRCClient *client = (IRCClient*)event->client->userdata;
	
	if (!client || !client->is_registered()) return;
	
	std::string chan_name = event->message->to<std::string>();
	std::string target_nick = event->message->to<std::string>();
	std::string reason = event->message->to();
	
	if (!chan_name.empty() && chan_name[0] == '#') {
		chan_name = chan_name.substr(1);
	}
	
	IRCChannel *channel = server->get_channel(chan_name);
	if (!channel) return;
	
	if (!channel->is_operator(client->get_fd())) {
		client->send(irc_err_chanoprivsneeded(client->nickname, chan_name));
		return;
	}
	
	IRCClient *target = server->get_client_by_nick(target_nick);
	if (!target) return;
	
	std::ostringstream oss;
	oss << ":" << client->nickname << " KICK #" << chan_name << " "
		<< target_nick << " :" << reason << "\r\n";
	
	channel->broadcast(oss.str());
	channel->skll_channel->remove_client(target->get_fd());
	target->part_channel(chan_name);
	
	irc_log_command("KICK", target_nick + " from #" + chan_name);
}

void irc_cmd_invite(SkllEvent *event, void *user_data) {
	IRCServer *server = (IRCServer*)user_data;
	IRCClient *client = (IRCClient*)event->client->userdata;
	
	if (!client || !client->is_registered()) return;
	
	std::string target_nick = event->message->to<std::string>();
	std::string chan_name = event->message->to<std::string>();
	
	if (!chan_name.empty() && chan_name[0] == '#') {
		chan_name = chan_name.substr(1);
	}
	
	IRCChannel *channel = server->get_channel(chan_name);
	IRCClient *target = server->get_client_by_nick(target_nick);
	
	if (!channel || !target) return;
	
	if (!channel->is_operator(client->get_fd())) {
		client->send(irc_err_chanoprivsneeded(client->nickname, chan_name));
		return;
	}
	
	channel->invite(target->get_fd());
	
	std::ostringstream oss;
	oss << ":" << client->nickname << " INVITE " << target_nick << " #" << chan_name << "\r\n";
	target->send(oss.str());
	
	irc_log_command("INVITE", target_nick + " to #" + chan_name);
}

void irc_cmd_topic(SkllEvent *event, void *user_data) {
	IRCServer *server = (IRCServer*)user_data;
	IRCClient *client = (IRCClient*)event->client->userdata;
	
	if (!client || !client->is_registered()) return;
	
	std::string chan_name = event->message->to<std::string>();
	std::string new_topic = event->message->to();
	
	if (!chan_name.empty() && chan_name[0] == '#') {
		chan_name = chan_name.substr(1);
	}
	
	IRCChannel *channel = server->get_channel(chan_name);
	if (!channel) return;
	
	if (channel->topic_protected && !channel->is_operator(client->get_fd())) {
		client->send(irc_err_chanoprivsneeded(client->nickname, chan_name));
		return;
	}
	
	channel->topic = new_topic;
	
	std::ostringstream oss;
	oss << ":" << client->nickname << " TOPIC #" << chan_name << " :" << new_topic << "\r\n";
	channel->broadcast(oss.str());
	
	irc_log_command("TOPIC", "#" + chan_name + " \"" + new_topic + "\"");
}

void irc_cmd_ping(SkllEvent *event, void *user_data) {
	(void)user_data;
	IRCClient *client = (IRCClient*)event->client->userdata;
	
	if (!client) return;
	
	std::string token = event->message->to();
	
	std::ostringstream oss;
	oss << ":server PONG server :" << token << "\r\n";
	client->send(oss.str());
	
	irc_log_command("PING", token);
}

void irc_cmd_quit(SkllEvent *event, void *user_data) {
	IRCServer *server = (IRCServer*)user_data;
	IRCClient *client = (IRCClient*)event->client->userdata;
	
	if (!client) return;
	
	std::string reason = event->message->to();
	if (reason.empty()) reason = "Client quit";

	std::ostringstream quit_msg;
	quit_msg << ":" << client->nickname << "!" << client->username << "@" << client->hostname
			 << " QUIT :" << reason << "\r\n";
	
	std::set<std::string>::iterator it;
	for (it = client->channels.begin(); it != client->channels.end(); ++it) {
		IRCChannel *channel = server->get_channel(*it);
		if (channel) {
			channel->broadcast(quit_msg.str(), client->get_fd());
		}
	}
	
	client->send("ERROR :Closing connection\r\n");
	
	irc_log("QUIT", client, reason);
	
	close(client->get_fd());
}
