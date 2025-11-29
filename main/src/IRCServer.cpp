/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/21 23:15:01 by marvin            #+#    #+#             */
/*   Updated: 2025/11/21 23:15:01 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include <IRCServer.hpp>
#include <sstream>
#include <cstdlib>
#include <algorithm>
#include <cctype>
#include <iomanip>
#include <ctime>

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   STATIC INSTANCE                                                           */
/* ═══════════════════════════════════════════════════════════════════════════ */

IRCServer *IRCServer::_instance = 0;
IRCServer *IRCServer::instance() { return _instance; }

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   PRETTY LOGGING                                                            */
/* ═══════════════════════════════════════════════════════════════════════════ */

static std::string timestamp() {
	std::time_t now = std::time(0);
	std::tm *t = std::localtime(&now);
	std::ostringstream oss;
	oss << std::setfill('0')
		<< std::setw(2) << t->tm_hour << ":"
		<< std::setw(2) << t->tm_min << ":"
		<< std::setw(2) << t->tm_sec;
	return oss.str();
}

static void log_connect(fd_t fd, const std::string &ip) {
	std::cout << SKLL_DIM << "[" << timestamp() << "]" << SKLL_RST << " "
			  << SKLL_GREEN << "| +" << SKLL_RST << " "
			  << SKLL_CYAN << "fd=" << fd << SKLL_RST << " "
			  << SKLL_DIM << "from" << SKLL_RST << " " << ip << std::endl;
}

static void log_disconnect(fd_t fd, const std::string &nick, const std::string &reason) {
	std::cout << SKLL_DIM << "[" << timestamp() << "]" << SKLL_RST << " "
			  << SKLL_RED << "| -" << SKLL_RST << " "
			  << SKLL_CYAN << "fd=" << fd << SKLL_RST << " "
			  << SKLL_YELLOW << nick << SKLL_RST << " "
			  << SKLL_DIM << "(" << reason << ")" << SKLL_RST << std::endl;
}

static void log_register(const std::string &nick, const std::string &user, const std::string &host) {
	std::cout << SKLL_DIM << "[" << timestamp() << "]" << SKLL_RST << " "
			  << SKLL_GREEN << "| ok" << SKLL_RST << " "
			  << SKLL_BOLD << nick << SKLL_RST << " "
			  << SKLL_DIM << "registered as" << SKLL_RST << " "
			  << user << "@" << host << std::endl;
}

static void log_join(const std::string &nick, const std::string &channel, bool created) {
	std::cout << SKLL_DIM << "[" << timestamp() << "]" << SKLL_RST << " "
			  << SKLL_CYAN << "| ->" << SKLL_RST << " "
			  << SKLL_YELLOW << nick << SKLL_RST << " "
			  << SKLL_DIM << "joined" << SKLL_RST << " "
			  << SKLL_BOLD << channel << SKLL_RST
			  << (created ? SKLL_GREEN " (created)" SKLL_RST : "") << std::endl;
}

static void log_part(const std::string &nick, const std::string &channel) {
	std::cout << SKLL_DIM << "[" << timestamp() << "]" << SKLL_RST << " "
			  << SKLL_MAGENTA << "| <-" << SKLL_RST << " "
			  << SKLL_YELLOW << nick << SKLL_RST << " "
			  << SKLL_DIM << "left" << SKLL_RST << " "
			  << channel << std::endl;
}

static void log_msg(const std::string &from, const std::string &to, const std::string &preview) {
	std::string short_msg = preview.length() > 40 ? preview.substr(0, 37) + "..." : preview;
	std::cout << SKLL_DIM << "[" << timestamp() << "]" << SKLL_RST << " "
			  << SKLL_BLUE << "| >>" << SKLL_RST << " "
			  << SKLL_YELLOW << from << SKLL_RST << " "
			  << SKLL_DIM << "->" << SKLL_RST << " "
			  << SKLL_CYAN << to << SKLL_RST << " "
			  << SKLL_DIM << "\"" << short_msg << "\"" << SKLL_RST << std::endl;
}

static void log_cmd(const std::string &nick, const std::string &cmd) {
	std::cout << SKLL_DIM << "[" << timestamp() << "]" << SKLL_RST << " "
			  << SKLL_DIM << "|   " << SKLL_RST
			  << SKLL_YELLOW << nick << SKLL_RST << " "
			  << SKLL_DIM << ":" << SKLL_RST << " "
			  << cmd << std::endl;
}

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   HELPERS                                                                   */
/* ═══════════════════════════════════════════════════════════════════════════ */

static std::string itoa(int n) {
	std::ostringstream oss;
	oss << n;
	return oss.str();
}

static std::string toupper_str(const std::string &s) {
	std::string r = s;
	for (size_t i = 0; i < r.size(); ++i)
		r[i] = static_cast<char>(std::toupper(static_cast<unsigned char>(r[i])));
	return r;
}

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   CONSTRUCTOR / DESTRUCTOR                                                  */
/* ═══════════════════════════════════════════════════════════════════════════ */

IRCServer::IRCServer(int port, const std::string &password)
	: _port(port), _password(password), _host("localhost"), _start_time(std::time(0))
{
	_instance = this;
}

IRCServer::~IRCServer() {
	_instance = 0;
}

std::string IRCServer::hostname() const { return _host; }

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   INTERNAL HELPERS                                                          */
/* ═══════════════════════════════════════════════════════════════════════════ */

IRCUser *IRCServer::_get_user(fd_t fd) {
	std::map<fd_t, IRCUser>::iterator it = _users.find(fd);
	return (it != _users.end()) ? &it->second : 0;
}

fd_t IRCServer::_find_by_nick(const std::string &nick) {
	for (std::map<fd_t, IRCUser>::iterator it = _users.begin(); it != _users.end(); ++it)
		if (it->second.nick == nick) return it->first;
	return -1;
}

std::string IRCServer::_prefix(IRCUser *u) {
	return u->nick + "!" + u->user + "@" + u->host;
}

void IRCServer::_send_num(SkllEvent &e, const char *code, const std::string &msg) {
	IRCUser *u = _get_user(e.fd());
	std::string nick = (u && !u->nick.empty()) ? u->nick : "*";
	e.send() << ":" << _host << " " << code << " " << nick << " " << msg;
}

void IRCServer::_broadcast(SkllChannel *ch, const std::string &msg, fd_t except) {
	if (!ch) return;
	const std::set<fd_t> &members = ch->members();
	for (std::set<fd_t>::const_iterator it = members.begin(); it != members.end(); ++it)
		if (*it != except)
			SkllSender(*it) << msg;
}

bool IRCServer::_check_registered(SkllEvent &e) {
	IRCUser *u = _get_user(e.fd());
	if (!u || !u->registered) {
		_send_num(e, ERR_NOTREGISTERED, ":You have not registered");
		e.send() << ":" << _host << " NOTICE *:Use: PASS <password>, NICK <nickname>, USER <username> 0 *:<realname>";
		return false;
	}
	return true;
}

void IRCServer::_welcome(SkllEvent &e, IRCUser *u) {
	_send_num(e, RPL_WELCOME, ":Welcome to the IRC Network " + _prefix(u));
	_send_num(e, RPL_YOURHOST, ":Your host is " + _host + ", running Sockell IRC v1.0");
	_send_num(e, RPL_CREATED, ":This server was created with Sockell");
	_send_num(e, RPL_MYINFO, _host + " SockellIRC-1.0 o itkol");
	
	/* Send helpful MOTD-style info */
	e.send() << ":" << _host << " 375 " << u->nick << " :- " << _host << " Message of the Day -";
	e.send() << ":" << _host << " 372 " << u->nick << " :- ";
	e.send() << ":" << _host << " 372 " << u->nick << " :-  Welcome to Sockell IRC Server!";
	e.send() << ":" << _host << " 372 " << u->nick << " :- ";
	e.send() << ":" << _host << " 372 " << u->nick << " :-  Quick start:";
	e.send() << ":" << _host << " 372 " << u->nick << " :-    JOIN #channel     - Join a channel";
	e.send() << ":" << _host << " 372 " << u->nick << " :-    PRIVMSG #ch :msg  - Send message";
	e.send() << ":" << _host << " 372 " << u->nick << " :-    !help             - Bot commands";
	e.send() << ":" << _host << " 372 " << u->nick << " :- ";
	e.send() << ":" << _host << " 372 " << u->nick << " :-  Type HELP for command list";
	e.send() << ":" << _host << " 372 " << u->nick << " :- ";
	e.send() << ":" << _host << " 376 " << u->nick << " :End of /MOTD command";
	
	log_register(u->nick, u->user, u->host);
}

void IRCServer::_send_names(SkllEvent &e, SkllChannel *ch) {
	if (!ch) return;
	std::string names;
	const std::set<fd_t> &members = ch->members();
	for (std::set<fd_t>::const_iterator it = members.begin(); it != members.end(); ++it) {
		IRCUser *m = _get_user(*it);
		if (m) {
			if (!names.empty()) names += " ";
			if (ch->has_perm(*it, IRC_PERM_OP)) names += "@";
			names += m->nick;
		}
	}
	_send_num(e, RPL_NAMREPLY, "= " + ch->name() + " :" + names);
	_send_num(e, RPL_ENDOFNAMES, ch->name() + " :End of /NAMES list");
}

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   WELCOME BANNER (sent on TCP connect)                                      */
/* ═══════════════════════════════════════════════════════════════════════════ */

void IRCServer::_send_banner(SkllEvent &e) {
	e.send() << ":" << _host << " NOTICE *:";
	e.send() << ":" << _host << " NOTICE *:+--------------------------------------------------+";
	e.send() << ":" << _host << " NOTICE *:|     SOCKELL IRC SERVER - ft_irc @ 42            |";
	e.send() << ":" << _host << " NOTICE *:+--------------------------------------------------+";
	e.send() << ":" << _host << " NOTICE *:";
	e.send() << ":" << _host << " NOTICE *:  To connect, use these commands:";
	e.send() << ":" << _host << " NOTICE *:";
	e.send() << ":" << _host << " NOTICE *:    PASS <password>           Set the password";
	e.send() << ":" << _host << " NOTICE *:    NICK <nickname>           Choose your nick";
	e.send() << ":" << _host << " NOTICE *:    USER <user> 0 *:<name>   Set username";
	e.send() << ":" << _host << " NOTICE *:";
	e.send() << ":" << _host << " NOTICE *:  Example:";
	e.send() << ":" << _host << " NOTICE *:    PASS secret123";
	e.send() << ":" << _host << " NOTICE *:    NICK john";
	e.send() << ":" << _host << " NOTICE *:    USER john 0 *:John Doe";
	e.send() << ":" << _host << " NOTICE *:";
	e.send() << ":" << _host << " NOTICE *:+--------------------------------------------------+";
	e.send() << ":" << _host << " NOTICE *:";
}

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   ROUTE SETUP                                                               */
/* ═══════════════════════════════════════════════════════════════════════════ */

void IRCServer::_setup_routes() {
	SkllRouter &r = _network.router();

	/*
	 *════════════════════════════════════════════════════════════════════════
	 *  REGISTRATION
	 *════════════════════════════════════════════════════════════════════════
	 */
	r.on<IRCServer>("PASS {*password}", _on_pass, this);
	r.on<IRCServer>("PASS", _on_pass_usage, this);
	r.on<IRCServer>("NICK {nick!}", _on_nick, this);
	r.on<IRCServer>("NICK", _on_nick_usage, this);
	r.on<IRCServer>("USER {user!} {mode} {unused} {*realname}", _on_user, this);
	r.on<IRCServer>("USER", _on_user_usage, this);

	/*
	 *════════════════════════════════════════════════════════════════════════
	 *  CHANNELS
	 *════════════════════════════════════════════════════════════════════════
	 */
	r.on<IRCServer>("JOIN {channel!} {key}", _on_join, this);
	r.on<IRCServer>("JOIN {channel!}", _on_join, this);
	r.on<IRCServer>("JOIN", _on_join_usage, this);
	r.on<IRCServer>("PART {channel!} {*reason}", _on_part, this);
	r.on<IRCServer>("PART {channel!}", _on_part, this);
	r.on<IRCServer>("PART", _on_part_usage, this);
	r.on<IRCServer>("TOPIC {channel!} {*newtopic}", _on_topic, this);
	r.on<IRCServer>("TOPIC {channel!}", _on_topic, this);
	r.on<IRCServer>("TOPIC", _on_topic_usage, this);
	r.on<IRCServer>("KICK {channel!} {target!} {*reason}", _on_kick, this);
	r.on<IRCServer>("KICK {channel!} {target!}", _on_kick, this);
	r.on<IRCServer>("KICK", _on_kick_usage, this);
	r.on<IRCServer>("INVITE {target!} {channel!}", _on_invite, this);
	r.on<IRCServer>("INVITE", _on_invite_usage, this);

	/*
	 *════════════════════════════════════════════════════════════════════════
	 *  MODES
	 *════════════════════════════════════════════════════════════════════════
	 */
	r.on<IRCServer>("MODE {target!} {modes} {*args}", _on_mode, this);
	r.on<IRCServer>("MODE {target!} {modes}", _on_mode, this);
	r.on<IRCServer>("MODE {target!}", _on_mode, this);
	r.on<IRCServer>("MODE", _on_mode_usage, this);

	/*
	 *════════════════════════════════════════════════════════════════════════
	 *  MESSAGING
	 *════════════════════════════════════════════════════════════════════════
	 */
	r.on<IRCServer>("PRIVMSG {target} {*text}", _on_privmsg, this);
	r.on<IRCServer>("NOTICE {target} {*text}", _on_notice, this);

	/*
	 *════════════════════════════════════════════════════════════════════════
	 *  UTILITY
	 *════════════════════════════════════════════════════════════════════════
	 */
	r.on<IRCServer>("PING {*token}", _on_ping, this);
	r.on<IRCServer>("PONG {*token}", _on_pong, this);
	r.on<IRCServer>("QUIT {*reason}", _on_quit, this);
	r.on<IRCServer>("WHO {mask}", _on_who, this);
	r.on<IRCServer>("WHO", _on_who_usage, this);
	r.on<IRCServer>("LIST", _on_list, this);
	r.on<IRCServer>("HELP", _on_help, this);
	r.on<IRCServer>("HELP {topic}", _on_help, this);

	/*
	 *════════════════════════════════════════════════════════════════════════
	 *  FALLBACK
	 *════════════════════════════════════════════════════════════════════════
	 */
	r.fallback<IRCServer>(_on_unknown, this);

	std::cout << SKLL_DIM << "[" << timestamp() << "]" << SKLL_RST << " "
			  << SKLL_GREEN << "| ok" << SKLL_RST << " "
			  << "Router: " << SKLL_CYAN << r.count() << SKLL_RST << " routes" << std::endl;
}

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   CONNECTION HANDLERS                                                       */
/* ═══════════════════════════════════════════════════════════════════════════ */

int IRCServer::_on_connect(SkllEvent &e, IRCServer *s) {
	IRCUser user;
	user.fd = e.fd();
	user.host = e.ip();
	user.pass_ok = s->_password.empty();
	user.registered = false;
	s->_users[e.fd()] = user;
	s->_send_banner(e);
	
	log_connect(e.fd(), e.ip());
	return SKLL_OK;
}

int IRCServer::_on_disconnect(SkllEvent &e, IRCServer *s) {
	IRCUser *u = s->_get_user(e.fd());
	std::string nick = u ? u->nick : "unknown";
	std::string reason = "connection closed";
	
	if (u && u->registered) {
		std::string msg = ":" + s->_prefix(u) + " QUIT :Connection lost";
		std::set<std::string> chans;
		s->_server.channels().of(e.fd(), chans);
		for (std::set<std::string>::iterator it = chans.begin(); it != chans.end(); ++it) {
			SkllChannel *ch = s->_server.channels().get(*it);
			if (ch) s->_broadcast(ch, msg, e.fd());
		}
	}
	s->_server.channels().remove_all(e.fd());
	s->_users.erase(e.fd());
	log_disconnect(e.fd(), nick, reason);
	return SKLL_STOP;
}

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   REGISTRATION COMMANDS                                                     */
/* ═══════════════════════════════════════════════════════════════════════════ */

int IRCServer::_on_pass(SkllEvent &e, IRCServer *s) {
	IRCUser *u = s->_get_user(e.fd());
	if (!u) return SKLL_ERR;
	if (u->registered) {
		s->_send_num(e, ERR_ALREADYREGISTRED, ":You may not reregister");
		return SKLL_OK;
	}
	std::string pass = e.message().param("password").str();
	if (!pass.empty() && pass[0] == ':') pass = pass.substr(1);
	
	if (pass == s->_password) {
		u->pass_ok = true;
		e.send() << ":" << s->_host << " NOTICE *:Password accepted. Now use NICK <nickname>";
		log_cmd(itoa(e.fd()), "PASS ****");
	} else {
		s->_send_num(e, ERR_PASSWDMISMATCH, ":Password incorrect");
		e.send() << ":" << s->_host << " NOTICE *:Wrong password. Try again: PASS <password>";
		log_cmd(itoa(e.fd()), "PASS **** " SKLL_RED "(wrong)" SKLL_RST);
	}
	return SKLL_OK;
}

int IRCServer::_on_pass_usage(SkllEvent &e, IRCServer *s) {
	s->_send_num(e, ERR_NEEDMOREPARAMS, "PASS :Not enough parameters");
	e.send() << ":" << s->_host << " NOTICE *:Usage: PASS <password>";
	return SKLL_OK;
}

int IRCServer::_on_nick_usage(SkllEvent &e, IRCServer *s) {
	s->_send_num(e, ERR_NONICKNAMEGIVEN, ":No nickname given");
	e.send() << ":" << s->_host << " NOTICE *:Usage: NICK <nickname>";
	e.send() << ":" << s->_host << " NOTICE *:  Nickname must start with a letter, max 9 chars";
	return SKLL_OK;
}

int IRCServer::_on_nick(SkllEvent &e, IRCServer *s) {
	IRCUser *u = s->_get_user(e.fd());
	if (!u) return SKLL_ERR;
	
	std::string new_nick = e.message().param("nick").str();
	if (new_nick.empty()) {
		s->_send_num(e, ERR_NONICKNAMEGIVEN, ":No nickname given");
		return SKLL_OK;
	}
	
	bool valid = std::isalpha(static_cast<unsigned char>(new_nick[0]));
	for (size_t i = 1; i < new_nick.size() && valid; ++i)
		valid = std::isalnum(static_cast<unsigned char>(new_nick[i])) || 
				new_nick[i] == '_' || new_nick[i] == '-' || new_nick[i] == '[' || 
				new_nick[i] == ']' || new_nick[i] == '\\' || new_nick[i] == '`' || 
				new_nick[i] == '^' || new_nick[i] == '{' || new_nick[i] == '}';
	
	if (!valid || new_nick.size() > 9) {
		s->_send_num(e, ERR_ERRONEUSNICKNAME, new_nick + " :Erroneous nickname");
		e.send() << ":" << s->_host << " NOTICE *:Nickname must start with a letter, contain only letters/numbers/special, max 9 chars";
		return SKLL_OK;
	}
	
	fd_t existing = s->_find_by_nick(new_nick);
	if (existing >= 0 && existing != e.fd()) {
		s->_send_num(e, ERR_NICKNAMEINUSE, new_nick + " :Nickname is already in use");
		e.send() << ":" << s->_host << " NOTICE *:Try another nickname: NICK <othernick>";
		return SKLL_OK;
	}
	
	std::string old_nick = u->nick;
	u->nick = new_nick;
	log_cmd(old_nick.empty() ? itoa(e.fd()) : old_nick, "NICK " + new_nick);
	
	if (u->registered) {
		std::string msg = ":" + old_nick + "!" + u->user + "@" + u->host + " NICK :" + new_nick;
		e.send() << msg;
		std::set<std::string> chans;
		s->_server.channels().of(e.fd(), chans);
		for (std::set<std::string>::iterator it = chans.begin(); it != chans.end(); ++it) {
			SkllChannel *ch = s->_server.channels().get(*it);
			if (ch) s->_broadcast(ch, msg, e.fd());
		}
	} else {
		if (!u->pass_ok) {
			e.send() << ":" << s->_host << " NOTICE *:Nick set to " << new_nick << ". But first: PASS <password>";
		} else if (u->user.empty()) {
			e.send() << ":" << s->_host << " NOTICE *:Nick set to " << new_nick << ". Now: USER " << new_nick << " 0 *:Your Name";
		}
	}
	
	if (!u->registered && u->pass_ok && !u->nick.empty() && !u->user.empty()) {
		u->registered = true;
		s->_welcome(e, u);
	}
	return SKLL_OK;
}

int IRCServer::_on_user_usage(SkllEvent &e, IRCServer *s) {
	IRCUser *u = s->_get_user(e.fd());
	std::string nick = (u && !u->nick.empty()) ? u->nick : "yournick";
	s->_send_num(e, ERR_NEEDMOREPARAMS, "USER :Not enough parameters");
	e.send() << ":" << s->_host << " NOTICE *:Usage: USER <username> 0 *:<realname>";
	e.send() << ":" << s->_host << " NOTICE *:Example: USER " << nick << " 0 *:John Doe";
	return SKLL_OK;
}

int IRCServer::_on_user(SkllEvent &e, IRCServer *s) {
	IRCUser *u = s->_get_user(e.fd());
	if (!u) return SKLL_ERR;
	if (u->registered) {
		s->_send_num(e, ERR_ALREADYREGISTRED, ":You may not reregister");
		return SKLL_OK;
	}
	
	u->user = e.message().param("user").str();
	std::string realname = e.message().param("realname").str();
	if (!realname.empty() && realname[0] == ':') realname = realname.substr(1);
	u->realname = realname;
	
	log_cmd(u->nick.empty() ? itoa(e.fd()) : u->nick, "USER " + u->user);
	
	if (u->pass_ok && !u->nick.empty() && !u->user.empty()) {
		u->registered = true;
		s->_welcome(e, u);
	} else if (!u->pass_ok) {
		e.send() << ":" << s->_host << " NOTICE *:USER set. But first: PASS <password>";
	} else if (u->nick.empty()) {
		e.send() << ":" << s->_host << " NOTICE *:USER set. Now: NICK <nickname>";
	}
	return SKLL_OK;
}

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   CHANNEL COMMANDS                                                          */
/* ═══════════════════════════════════════════════════════════════════════════ */

int IRCServer::_on_join_usage(SkllEvent &e, IRCServer *s) {
	if (!s->_check_registered(e)) return SKLL_OK;
	s->_send_num(e, ERR_NEEDMOREPARAMS, "JOIN :Not enough parameters");
	e.send() << ":" << s->_host << " NOTICE *:Usage: JOIN #channel [key]";
	e.send() << ":" << s->_host << " NOTICE *:Example: JOIN #general";
	return SKLL_OK;
}

int IRCServer::_on_join(SkllEvent &e, IRCServer *s) {
	if (!s->_check_registered(e)) return SKLL_OK;
	IRCUser *u = s->_get_user(e.fd());
	std::string chan_name = e.message().param("channel").str();
	std::string key = e.message().param("key").str();

	if (chan_name.empty() || (chan_name[0] != '#' && chan_name[0] != '&')) {
		s->_send_num(e, ERR_NOSUCHCHANNEL, chan_name + " :Invalid channel name");
		e.send() << ":" << s->_host << " NOTICE *:Channel names must start with # or &";
		return SKLL_OK;
	}

	SkllChannel *ch = s->_server.channels().get(chan_name);
	bool created = false;
	size_t perms = 0;

	if (!ch) {
		ch = s->_server.channels().create(chan_name);
		created = true;
		perms = IRC_PERM_OP;
	} else {
		if (ch->get("mode_i") == "1" && !ch->has_perm(e.fd(), IRC_PERM_INVITED)) {
			s->_send_num(e, ERR_INVITEONLYCHAN, chan_name + " :Cannot join channel (+i)");
			e.send() << ":" << s->_host << " NOTICE *:This channel is invite-only. Ask an operator to invite you.";
			return SKLL_OK;
		}
		if (!ch->get("key").empty() && ch->get("key") != key) {
			s->_send_num(e, ERR_BADCHANNELKEY, chan_name + " :Cannot join channel (+k)");
			e.send() << ":" << s->_host << " NOTICE *:Wrong channel key. Use: JOIN " << chan_name << " <key>";
			return SKLL_OK;
		}
		std::string limit_str = ch->get("limit");
		if (!limit_str.empty()) {
			int limit = std::atoi(limit_str.c_str());
			if (limit > 0 && ch->count() >= static_cast<size_t>(limit)) {
				s->_send_num(e, ERR_CHANNELISFULL, chan_name + " :Cannot join channel (+l)");
				e.send() << ":" << s->_host << " NOTICE *:Channel is full (limit: " << limit << ")";
				return SKLL_OK;
			}
		}
	}

	s->_server.channels().join(e.fd(), chan_name, perms);
	ch->del_perms(e.fd(), IRC_PERM_INVITED);

	std::string msg = ":" + s->_prefix(u) + " JOIN " + chan_name;
	s->_broadcast(ch, msg, -1);

	std::string topic = ch->get("topic");
	if (!topic.empty())
		s->_send_num(e, RPL_TOPIC, chan_name + " :" + topic);
	else
		s->_send_num(e, RPL_NOTOPIC, chan_name + " :No topic is set");

	s->_send_names(e, ch);
	
	if (created) {
		e.send() << ":" << s->_host << " NOTICE " << u->nick << " :You created " << chan_name << " and are now operator (@)";
		e.send() << ":" << s->_host << " NOTICE " << u->nick << " :Use TOPIC " << chan_name << " :text to set a topic";
	}
	
	log_join(u->nick, chan_name, created);
	return SKLL_OK;
}

int IRCServer::_on_part_usage(SkllEvent &e, IRCServer *s) {
	if (!s->_check_registered(e)) return SKLL_OK;
	s->_send_num(e, ERR_NEEDMOREPARAMS, "PART :Not enough parameters");
	e.send() << ":" << s->_host << " NOTICE *:Usage: PART #channel [:reason]";
	return SKLL_OK;
}

int IRCServer::_on_part(SkllEvent &e, IRCServer *s) {
	if (!s->_check_registered(e)) return SKLL_OK;
	IRCUser *u = s->_get_user(e.fd());
	std::string chan_name = e.message().param("channel").str();
	std::string reason = e.message().param("reason").str();
	if (!reason.empty() && reason[0] == ':') reason = reason.substr(1);

	SkllChannel *ch = s->_server.channels().get(chan_name);
	if (!ch || !ch->has(e.fd())) {
		s->_send_num(e, ERR_NOTONCHANNEL, chan_name + " :You're not on that channel");
		return SKLL_OK;
	}

	std::string msg = ":" + s->_prefix(u) + " PART " + chan_name;
	if (!reason.empty()) msg += " :" + reason;
	s->_broadcast(ch, msg, -1);

	s->_server.channels().part(e.fd(), chan_name);
	if (ch->empty()) s->_server.channels().remove(chan_name);

	log_part(u->nick, chan_name);
	return SKLL_OK;
}

int IRCServer::_on_topic_usage(SkllEvent &e, IRCServer *s) {
	if (!s->_check_registered(e)) return SKLL_OK;
	s->_send_num(e, ERR_NEEDMOREPARAMS, "TOPIC :Not enough parameters");
	e.send() << ":" << s->_host << " NOTICE *:Usage: TOPIC #channel [:new topic]";
	e.send() << ":" << s->_host << " NOTICE *:  Without text: show current topic";
	e.send() << ":" << s->_host << " NOTICE *:  With text: set new topic (if operator)";
	return SKLL_OK;
}

int IRCServer::_on_topic(SkllEvent &e, IRCServer *s) {
	if (!s->_check_registered(e)) return SKLL_OK;
	IRCUser *u = s->_get_user(e.fd());
	std::string chan_name = e.message().param("channel").str();

	SkllChannel *ch = s->_server.channels().get(chan_name);
	if (!ch) {
		s->_send_num(e, ERR_NOSUCHCHANNEL, chan_name + " :No such channel");
		return SKLL_OK;
	}
	if (!ch->has(e.fd())) {
		s->_send_num(e, ERR_NOTONCHANNEL, chan_name + " :You're not on that channel");
		return SKLL_OK;
	}

	if (!e.message().has_param("newtopic")) {
		std::string topic = ch->get("topic");
		if (topic.empty())
			s->_send_num(e, RPL_NOTOPIC, chan_name + " :No topic is set");
		else
			s->_send_num(e, RPL_TOPIC, chan_name + " :" + topic);
	} else {
		if (ch->get("mode_t") == "1" && !ch->has_perm(e.fd(), IRC_PERM_OP)) {
			s->_send_num(e, ERR_CHANOPRIVSNEEDED, chan_name + " :You're not channel operator");
			e.send() << ":" << s->_host << " NOTICE *:Channel has +t mode. Only operators can change topic.";
			return SKLL_OK;
		}
		std::string new_topic = e.message().param("newtopic").str();
		if (!new_topic.empty() && new_topic[0] == ':') new_topic = new_topic.substr(1);
		ch->set("topic", new_topic);
		s->_broadcast(ch, ":" + s->_prefix(u) + " TOPIC " + chan_name + " :" + new_topic, -1);
		log_cmd(u->nick, "TOPIC " + chan_name + " \"" + new_topic + "\"");
	}
	return SKLL_OK;
}

int IRCServer::_on_kick_usage(SkllEvent &e, IRCServer *s) {
	if (!s->_check_registered(e)) return SKLL_OK;
	s->_send_num(e, ERR_NEEDMOREPARAMS, "KICK :Not enough parameters");
	e.send() << ":" << s->_host << " NOTICE *:Usage: KICK #channel nick [:reason]";
	e.send() << ":" << s->_host << " NOTICE *:  Requires operator status (@)";
	return SKLL_OK;
}

int IRCServer::_on_kick(SkllEvent &e, IRCServer *s) {
	if (!s->_check_registered(e)) return SKLL_OK;
	IRCUser *u = s->_get_user(e.fd());
	std::string chan_name = e.message().param("channel").str();
	std::string target_nick = e.message().param("target").str();
	std::string reason = e.message().has_param("reason") ? e.message().param("reason").str() : u->nick;
	if (!reason.empty() && reason[0] == ':') reason = reason.substr(1);

	SkllChannel *ch = s->_server.channels().get(chan_name);
	if (!ch) {
		s->_send_num(e, ERR_NOSUCHCHANNEL, chan_name + " :No such channel");
		return SKLL_OK;
	}
	if (!ch->has_perm(e.fd(), IRC_PERM_OP)) {
		s->_send_num(e, ERR_CHANOPRIVSNEEDED, chan_name + " :You're not channel operator");
		return SKLL_OK;
	}
	fd_t target_fd = s->_find_by_nick(target_nick);
	if (target_fd < 0 || !ch->has(target_fd)) {
		s->_send_num(e, ERR_USERNOTINCHANNEL, target_nick + " " + chan_name + " :They aren't on that channel");
		return SKLL_OK;
	}

	s->_broadcast(ch, ":" + s->_prefix(u) + " KICK " + chan_name + " " + target_nick + " :" + reason, -1);
	s->_server.channels().part(target_fd, chan_name);
	log_cmd(u->nick, "KICK " + target_nick + " from " + chan_name);
	return SKLL_OK;
}

int IRCServer::_on_invite_usage(SkllEvent &e, IRCServer *s) {
	if (!s->_check_registered(e)) return SKLL_OK;
	s->_send_num(e, ERR_NEEDMOREPARAMS, "INVITE :Not enough parameters");
	e.send() << ":" << s->_host << " NOTICE *:Usage: INVITE nick #channel";
	e.send() << ":" << s->_host << " NOTICE *:  Invite someone to a channel (useful for +i channels)";
	return SKLL_OK;
}

int IRCServer::_on_invite(SkllEvent &e, IRCServer *s) {
	if (!s->_check_registered(e)) return SKLL_OK;
	IRCUser *u = s->_get_user(e.fd());
	std::string target_nick = e.message().param("target").str();
	std::string chan_name = e.message().param("channel").str();

	fd_t target_fd = s->_find_by_nick(target_nick);
	if (target_fd < 0) {
		s->_send_num(e, ERR_NOSUCHNICK, target_nick + " :No such nick");
		return SKLL_OK;
	}

	SkllChannel *ch = s->_server.channels().get(chan_name);
	if (!ch || !ch->has(e.fd())) {
		s->_send_num(e, ERR_NOTONCHANNEL, chan_name + " :You're not on that channel");
		return SKLL_OK;
	}
	if (ch->get("mode_i") == "1" && !ch->has_perm(e.fd(), IRC_PERM_OP)) {
		s->_send_num(e, ERR_CHANOPRIVSNEEDED, chan_name + " :You're not channel operator");
		return SKLL_OK;
	}
	if (ch->has(target_fd)) {
		s->_send_num(e, ERR_USERONCHANNEL, target_nick + " " + chan_name + " :is already on channel");
		return SKLL_OK;
	}

	ch->add(target_fd, IRC_PERM_INVITED);
	s->_send_num(e, RPL_INVITING, chan_name + " " + target_nick);
	SkllSender(target_fd) << ":" << s->_prefix(u) << " INVITE " << target_nick << " :" << chan_name;
	log_cmd(u->nick, "INVITE " + target_nick + " to " + chan_name);
	return SKLL_OK;
}

int IRCServer::_on_mode_usage(SkllEvent &e, IRCServer *s) {
	if (!s->_check_registered(e)) return SKLL_OK;
	s->_send_num(e, ERR_NEEDMOREPARAMS, "MODE :Not enough parameters");
	e.send() << ":" << s->_host << " NOTICE *:Usage: MODE #channel [+/-modes] [args]";
	e.send() << ":" << s->_host << " NOTICE *:Modes: i (invite-only), t (topic lock), k (key), l (limit), o (operator)";
	e.send() << ":" << s->_host << " NOTICE *:Examples:";
	e.send() << ":" << s->_host << " NOTICE *:  MODE #chan +i           Set invite-only";
	e.send() << ":" << s->_host << " NOTICE *:  MODE #chan +k secret    Set channel key";
	e.send() << ":" << s->_host << " NOTICE *:  MODE #chan +o nick      Give operator to nick";
	return SKLL_OK;
}

int IRCServer::_on_mode(SkllEvent &e, IRCServer *s) {
	if (!s->_check_registered(e)) return SKLL_OK;
	IRCUser *u = s->_get_user(e.fd());
	std::string target = e.message().param("target").str();

	if (target[0] != '#' && target[0] != '&') {
		s->_send_num(e, RPL_UMODEIS, "+");
		return SKLL_OK;
	}

	SkllChannel *ch = s->_server.channels().get(target);
	if (!ch) {
		s->_send_num(e, ERR_NOSUCHCHANNEL, target + " :No such channel");
		return SKLL_OK;
	}

	if (!e.message().has_param("modes")) {
		std::string modes = "+";
		if (ch->get("mode_i") == "1") modes += "i";
		if (ch->get("mode_t") == "1") modes += "t";
		if (!ch->get("key").empty()) modes += "k";
		if (!ch->get("limit").empty()) modes += "l";
		s->_send_num(e, RPL_CHANNELMODEIS, target + " " + modes);
		return SKLL_OK;
	}

	if (!ch->has_perm(e.fd(), IRC_PERM_OP)) {
		s->_send_num(e, ERR_CHANOPRIVSNEEDED, target + " :You're not channel operator");
		return SKLL_OK;
	}

	std::string mode_str = e.message().param("modes").str();
	std::string args_str = e.message().param("args").str();
	if (!args_str.empty() && args_str[0] == ':') args_str = args_str.substr(1);
	
	std::vector<std::string> args;
	std::istringstream iss(args_str);
	std::string arg;
	while (iss >> arg) args.push_back(arg);
	size_t arg_idx = 0;

	bool adding = true;
	std::string applied_modes, applied_args;
	
	for (size_t i = 0; i < mode_str.size(); ++i) {
		char c = mode_str[i];
		if (c == '+') { adding = true; continue; }
		if (c == '-') { adding = false; continue; }

		switch (c) {
			case 'i':
				ch->set("mode_i", adding ? "1" : "");
				applied_modes += (adding ? "+i" : "-i");
				break;
			case 't':
				ch->set("mode_t", adding ? "1" : "");
				applied_modes += (adding ? "+t" : "-t");
				break;
			case 'k':
				if (adding && arg_idx < args.size()) {
					ch->set("key", args[arg_idx]);
					applied_modes += "+k";
					applied_args += " " + args[arg_idx++];
				} else if (!adding) {
					ch->del("key");
					applied_modes += "-k";
				}
				break;
			case 'l':
				if (adding && arg_idx < args.size()) {
					ch->set("limit", args[arg_idx]);
					applied_modes += "+l";
					applied_args += " " + args[arg_idx++];
				} else if (!adding) {
					ch->del("limit");
					applied_modes += "-l";
				}
				break;
			case 'o':
				if (arg_idx < args.size()) {
					fd_t op_fd = s->_find_by_nick(args[arg_idx]);
					if (op_fd >= 0 && ch->has(op_fd)) {
						if (adding) ch->add_perms(op_fd, IRC_PERM_OP);
						else ch->del_perms(op_fd, IRC_PERM_OP);
						applied_modes += (adding ? "+o" : "-o");
						applied_args += " " + args[arg_idx];
					}
					++arg_idx;
				}
				break;
			default:
				s->_send_num(e, ERR_UNKNOWNMODE, std::string(1, c) + " :is unknown mode char");
		}
	}

	if (!applied_modes.empty()) {
		s->_broadcast(ch, ":" + s->_prefix(u) + " MODE " + target + " " + applied_modes + applied_args, -1);
		log_cmd(u->nick, "MODE " + target + " " + applied_modes);
	}
	return SKLL_OK;
}

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   MESSAGING                                                                 */
/* ═══════════════════════════════════════════════════════════════════════════ */

int IRCServer::_on_privmsg_usage(SkllEvent &e, IRCServer *s) {
	if (!s->_check_registered(e)) return SKLL_OK;
	s->_send_num(e, ERR_NORECIPIENT, ":No recipient given (PRIVMSG)");
	e.send() << ":" << s->_host << " NOTICE *:Usage: PRIVMSG target :message";
	e.send() << ":" << s->_host << " NOTICE *:  target = #channel or nickname";
	e.send() << ":" << s->_host << " NOTICE *:Example: PRIVMSG #general :Hello everyone!";
	return SKLL_OK;
}

int IRCServer::_on_privmsg(SkllEvent &e, IRCServer *s) {
	if (!s->_check_registered(e)) return SKLL_OK;
	IRCUser *u = s->_get_user(e.fd());
	std::string target = e.message().param("target").str();
	std::string text = e.message().param("text").str();
	if (target.empty() || text.empty())
		_on_privmsg_usage(e, s);
	if (!text.empty() && text[0] == ':') text = text.substr(1);

	if (text.empty()) {
		s->_send_num(e, ERR_NOTEXTTOSEND, ":No text to send");
		return SKLL_OK;
	}

	/* Bot commands - works anywhere */
	if (text[0] == '!') {
		std::string reply_to = (target[0] == '#' || target[0] == '&') ? target : u->nick;
		
		if (text == "!help") {
			e.send() << ":SockBot!bot@" << s->_host << " PRIVMSG " << reply_to 
					 << " :=== SockBot Commands ===";
			e.send() << ":SockBot!bot@" << s->_host << " PRIVMSG " << reply_to 
					 << " :!help   - Show this help";
			e.send() << ":SockBot!bot@" << s->_host << " PRIVMSG " << reply_to 
					 << " :!time   - Current server time";
			e.send() << ":SockBot!bot@" << s->_host << " PRIVMSG " << reply_to 
					 << " :!stats  - Server statistics";
			e.send() << ":SockBot!bot@" << s->_host << " PRIVMSG " << reply_to 
					 << " :!roll   - Roll a dice (1-6)";
			e.send() << ":SockBot!bot@" << s->_host << " PRIVMSG " << reply_to 
					 << " :!uptime - Server uptime";
			log_cmd(u->nick, "!help");
			return SKLL_OK;
		}
		if (text == "!time") {
			std::time_t now = std::time(0);
			char buf[64];
			std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
			e.send() << ":SockBot!bot@" << s->_host << " PRIVMSG " << reply_to 
					 << " :Current time: " << buf;
			log_cmd(u->nick, "!time");
			return SKLL_OK;
		}
		if (text == "!stats") {
			e.send() << ":SockBot!bot@" << s->_host << " PRIVMSG " << reply_to 
					 << " :Users online: " << s->_users.size() << " | Channels: " << s->_server.channels().count();
			log_cmd(u->nick, "!stats");
			return SKLL_OK;
		}
		if (text == "!roll") {
			int roll = (std::rand() % 6) + 1;
			e.send() << ":SockBot!bot@" << s->_host << " PRIVMSG " << reply_to 
					 << " :" << u->nick << " rolled a " << roll << "!";
			log_cmd(u->nick, "!roll -> " + itoa(roll));
			return SKLL_OK;
		}
		if (text == "!uptime") {
			time_t diff = std::time(0) - s->_start_time;
			int d = diff / 86400, h = (diff % 86400) / 3600, m = (diff % 3600) / 60, sec = diff % 60;
			std::ostringstream oss;
			if (d) oss << d << "d ";
			if (h) oss << h << "h ";
			if (m) oss << m << "m ";
			oss << sec << "s";
			e.send() << ":SockBot!bot@" << s->_host << " PRIVMSG " << reply_to 
					 << " :Uptime: " << oss.str();
			log_cmd(u->nick, "!uptime");
			return SKLL_OK;
		}
	}

	std::string msg = ":" + s->_prefix(u) + " PRIVMSG " + target + " :" + text;

	if (target[0] == '#' || target[0] == '&') {
		SkllChannel *ch = s->_server.channels().get(target);
		if (!ch) {
			s->_send_num(e, ERR_NOSUCHCHANNEL, target + " :No such channel");
			return SKLL_OK;
		}
		if (!ch->has(e.fd())) {
			s->_send_num(e, ERR_CANNOTSENDTOCHAN, target + " :Cannot send to channel");
			e.send() << ":" << s->_host << " NOTICE *:You must JOIN " << target << " first";
			return SKLL_OK;
		}
		s->_broadcast(ch, msg, e.fd());
		log_msg(u->nick, target, text);
	} else {
		fd_t target_fd = s->_find_by_nick(target);
		if (target_fd < 0) {
			s->_send_num(e, ERR_NOSUCHNICK, target + " :No such nick");
			return SKLL_OK;
		}
		SkllSender(target_fd) << msg;
		log_msg(u->nick, target, text);
	}
	return SKLL_OK;
}

int IRCServer::_on_notice_usage(SkllEvent &e, IRCServer *s) {
	if (!s->_check_registered(e)) return SKLL_OK;
	e.send() << ":" << s->_host << " NOTICE *:Usage: NOTICE target :message";
	return SKLL_OK;
}

int IRCServer::_on_notice(SkllEvent &e, IRCServer *s) {
	if (!s->_check_registered(e)) return SKLL_OK;
	IRCUser *u = s->_get_user(e.fd());
	std::string target = e.message().param("target").str();
	std::string text = e.message().param("text").str();
	if (target.empty() || text.empty())
		_on_notice_usage(e, s);
	if (!text.empty() && text[0] == ':') text = text.substr(1);

	std::string msg = ":" + s->_prefix(u) + " NOTICE " + target + " :" + text;

	if (target[0] == '#' || target[0] == '&') {
		SkllChannel *ch = s->_server.channels().get(target);
		if (ch && ch->has(e.fd()))
			s->_broadcast(ch, msg, e.fd());
	} else {
		fd_t target_fd = s->_find_by_nick(target);
		if (target_fd >= 0)
			SkllSender(target_fd) << msg;
	}
	return SKLL_OK;
}

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   UTILITY COMMANDS                                                          */
/* ═══════════════════════════════════════════════════════════════════════════ */

int IRCServer::_on_ping(SkllEvent &e, IRCServer *s) {
	std::string token = e.message().param("token").str();
	if (!token.empty() && token[0] == ':') token = token.substr(1);
	e.send() << ":" << s->_host << " PONG " << s->_host << " :" << token;
	return SKLL_OK;
}

int IRCServer::_on_pong(SkllEvent &, IRCServer *) {
	return SKLL_OK;
}

int IRCServer::_on_quit(SkllEvent &e, IRCServer *s) {
	IRCUser *u = s->_get_user(e.fd());
	std::string reason = e.message().has_param("reason") ? e.message().param("reason").str() : "Client quit";
	if (!reason.empty() && reason[0] == ':') reason = reason.substr(1);

	std::string msg = ":" + s->_prefix(u) + " QUIT :" + reason;
	std::set<std::string> chans;
	s->_server.channels().of(e.fd(), chans);
	for (std::set<std::string>::iterator it = chans.begin(); it != chans.end(); ++it) {
		SkllChannel *ch = s->_server.channels().get(*it);
		if (ch) s->_broadcast(ch, msg, e.fd());
	}
	log_disconnect(e.fd(), u->nick, reason);
	e.send() << "ERROR :Closing link (" << reason << ")";
	return SKLL_STOP;
}

int IRCServer::_on_who_usage(SkllEvent &e, IRCServer *s) {
	if (!s->_check_registered(e)) return SKLL_OK;
	e.send() << ":" << s->_host << " NOTICE *:Usage: WHO #channel";
	e.send() << ":" << s->_host << " NOTICE *:  Lists users in a channel";
	return SKLL_OK;
}

int IRCServer::_on_who(SkllEvent &e, IRCServer *s) {
	if (!s->_check_registered(e)) return SKLL_OK;
	std::string mask = e.message().param("mask").str();

	SkllChannel *ch = s->_server.channels().get(mask);
	if (ch) {
		const std::set<fd_t> &members = ch->members();
		for (std::set<fd_t>::const_iterator it = members.begin(); it != members.end(); ++it) {
			IRCUser *m = s->_get_user(*it);
			if (m) {
				std::string flags = "H";
				if (ch->has_perm(*it, IRC_PERM_OP)) flags += "@";
				s->_send_num(e, RPL_WHOREPLY, mask + " " + m->user + " " + m->host + " " + 
							 s->_host + " " + m->nick + " " + flags + " :0 " + m->realname);
			}
		}
	}
	s->_send_num(e, RPL_ENDOFWHO, mask + " :End of /WHO list");
	return SKLL_OK;
}

int IRCServer::_on_list(SkllEvent &e, IRCServer *s) {
	if (!s->_check_registered(e)) return SKLL_OK;
	IRCUser *u = s->_get_user(e.fd());
	
	e.send() << ":" << s->_host << " 321 " << u->nick << " Channel :Users  Name";
	
	/* List all channels */
	const std::map<std::string, SkllChannel > &chans = s->_server.channels().all();
	for (std::map<std::string, SkllChannel >::const_iterator it = chans.begin(); it != chans.end(); ++it) {
		std::string topic = it->second.get("topic");
		e.send() << ":" << s->_host << " 322 " << u->nick << " " << it->first << " " 
				 << it->second.count() << " :" << topic;
	}
	
	e.send() << ":" << s->_host << " 323 " << u->nick << " :End of /LIST";
	return SKLL_OK;
}

int IRCServer::_on_help(SkllEvent &e, IRCServer *s) {
	if (!s->_check_registered(e)) return SKLL_OK;
	IRCUser *u = s->_get_user(e.fd());
	
	std::string topic = e.message().param("topic").str();
	
	if (topic.empty()) {
		e.send() << ":" << s->_host << " NOTICE " << u->nick << " :=== IRC Commands ===";
		e.send() << ":" << s->_host << " NOTICE " << u->nick << " :";
		e.send() << ":" << s->_host << " NOTICE " << u->nick << " :CHANNELS:";
		e.send() << ":" << s->_host << " NOTICE " << u->nick << " :  JOIN #channel      - Join a channel";
		e.send() << ":" << s->_host << " NOTICE " << u->nick << " :  PART #channel      - Leave a channel";
		e.send() << ":" << s->_host << " NOTICE " << u->nick << " :  TOPIC #channel     - View/set topic";
		e.send() << ":" << s->_host << " NOTICE " << u->nick << " :  LIST               - List all channels";
		e.send() << ":" << s->_host << " NOTICE " << u->nick << " :";
		e.send() << ":" << s->_host << " NOTICE " << u->nick << " :MESSAGING:";
		e.send() << ":" << s->_host << " NOTICE " << u->nick << " :  PRIVMSG target :msg - Send message";
		e.send() << ":" << s->_host << " NOTICE " << u->nick << " :  NOTICE target :msg  - Send notice";
		e.send() << ":" << s->_host << " NOTICE " << u->nick << " :";
		e.send() << ":" << s->_host << " NOTICE " << u->nick << " :OPERATOR (need @):";
		e.send() << ":" << s->_host << " NOTICE " << u->nick << " :  KICK #ch nick      - Kick user";
		e.send() << ":" << s->_host << " NOTICE " << u->nick << " :  INVITE nick #ch    - Invite user";
		e.send() << ":" << s->_host << " NOTICE " << u->nick << " :  MODE #ch +modes    - Set modes";
		e.send() << ":" << s->_host << " NOTICE " << u->nick << " :";
		e.send() << ":" << s->_host << " NOTICE " << u->nick << " :BOT (!commands in any channel):";
		e.send() << ":" << s->_host << " NOTICE " << u->nick << " :  !help !time !stats !roll !uptime";
		e.send() << ":" << s->_host << " NOTICE " << u->nick << " :";
		e.send() << ":" << s->_host << " NOTICE " << u->nick << " :Type HELP <command> for details";
	} else {
		std::string cmd = toupper_str(topic);
		e.send() << ":" << s->_host << " NOTICE " << u->nick << " :Help for " << cmd << ":";
		
		if (cmd == "JOIN") {
			e.send() << ":" << s->_host << " NOTICE " << u->nick << " :  JOIN #channel [key]";
			e.send() << ":" << s->_host << " NOTICE " << u->nick << " :  Creates or joins a channel. Creator gets @ (operator)";
		} else if (cmd == "MODE") {
			e.send() << ":" << s->_host << " NOTICE " << u->nick << " :  MODE #channel [+/-modes] [args]";
			e.send() << ":" << s->_host << " NOTICE " << u->nick << " :  Modes: i=invite-only, t=topic-lock, k=key, l=limit, o=op";
		} else if (cmd == "PRIVMSG") {
			e.send() << ":" << s->_host << " NOTICE " << u->nick << " :  PRIVMSG target :message";
			e.send() << ":" << s->_host << " NOTICE " << u->nick << " :  Target can be #channel or nickname";
		} else {
			e.send() << ":" << s->_host << " NOTICE " << u->nick << " :  No help available for " << cmd;
		}
	}
	return SKLL_OK;
}

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   FALLBACK                                                                  */
/* ═══════════════════════════════════════════════════════════════════════════ */

int IRCServer::_on_unknown(SkllEvent &e, IRCServer *s) {
	IRCUser *u = s->_get_user(e.fd());
	std::string cmd = e.command().str();
	
	if (cmd.empty()) return SKLL_OK;
	
	std::string upper_cmd = toupper_str(cmd);
	
	if (u && u->registered) {
		s->_send_num(e, ERR_UNKNOWNCOMMAND, upper_cmd + " :Unknown command");
		e.send() << ":" << s->_host << " NOTICE " << u->nick << " :Type HELP for a list of commands";
		log_cmd(u->nick, cmd + " (unknown)");
	} else {
		/* Not registered yet - give guidance */
		s->_send_num(e, ERR_NOTREGISTERED, ":You have not registered");
		e.send() << ":" << s->_host << " NOTICE *:You must register first:";
		e.send() << ":" << s->_host << " NOTICE *:  1. PASS <password>";
		e.send() << ":" << s->_host << " NOTICE *:  2. NICK <nickname>";
		e.send() << ":" << s->_host << " NOTICE *:  3. USER <name> 0 *:<realname>";
	}
	return SKLL_OK;
}

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   SERVER LIFECYCLE                                                          */
/* ═══════════════════════════════════════════════════════════════════════════ */

bool IRCServer::start() {
	std::cout << std::endl;
	std::cout << SKLL_CYAN << "+--------------------------------------------------------------+" << SKLL_RST << std::endl;
	std::cout << SKLL_CYAN << "|" << SKLL_RST << "                                                              " << SKLL_CYAN << "|" << SKLL_RST << std::endl;
	std::cout << SKLL_CYAN << "|" << SKLL_RST << "     " << SKLL_BOLD << SKLL_WHITE << "IRC SERVER" << SKLL_RST << "   -----------------------   " << SKLL_DIM << "ft_irc" << SKLL_RST << "     " << SKLL_CYAN << "|" << SKLL_RST << std::endl;
	std::cout << SKLL_CYAN << "|" << SKLL_RST << "                  " << SKLL_DIM << "Powered by Sockell" << SKLL_RST << "                      " << SKLL_CYAN << "|" << SKLL_RST << std::endl;
	std::cout << SKLL_CYAN << "|" << SKLL_RST << "                                                              " << SKLL_CYAN << "|" << SKLL_RST << std::endl;
	std::cout << SKLL_CYAN << "+--------------------------------------------------------------+" << SKLL_RST << std::endl;
	std::cout << std::endl;

	_proto.set_flags(SKLL_TCP | SKLL_IPV4);
	_proto.set_port(_port);
	_proto.set_host("0.0.0.0");
	_proto.set_reuseaddr(true);
	_proto.set_nodelay(true);

	_network.set_name("irc");
	_network.set_delim("\r\n");
	_network.add(_proto);
	_network.on<IRCServer>(SKLL_EV_ACCEPT, _on_connect, this);
	_network.on<IRCServer>(SKLL_EV_CLOSE, _on_disconnect, this);

	_setup_routes();

	_server.add(_network);
	_server.signals().trap_int().trap_term().trap_pipe();

	if (!_server.init()) {
		std::cerr << SKLL_RED << "Failed to initialize server" << SKLL_RST << std::endl;
		return false;
	}

	std::cout << SKLL_DIM << "[" << timestamp() << "]" << SKLL_RST << " "
			  << SKLL_GREEN << "| ok" << SKLL_RST << " "
			  << "Listening on " << SKLL_CYAN << "0.0.0.0:" << _port << SKLL_RST << std::endl;
	std::cout << SKLL_DIM << "[" << timestamp() << "]" << SKLL_RST << " "
			  << SKLL_GREEN << "| ok" << SKLL_RST << " "
			  << "Password: " << SKLL_YELLOW << "****" << SKLL_RST << " (" << _password.length() << " chars)" << std::endl;
	std::cout << SKLL_DIM << "[" << timestamp() << "]" << SKLL_RST << " "
			  << SKLL_CYAN << "| i " << SKLL_RST
			  << "Bot: " << SKLL_BOLD << "SockBot" << SKLL_RST << " (!help in any channel)" << std::endl;
	std::cout << SKLL_DIM << "[" << timestamp() << "]" << SKLL_RST << " "
			  << SKLL_DIM << "|   " << SKLL_RST
			  << "Press " << SKLL_YELLOW << "Ctrl+C" << SKLL_RST << " to shutdown" << std::endl;
	std::cout << std::endl;
	std::cout << SKLL_DIM << "---------------------------------------------------------------" << SKLL_RST << std::endl;
	std::cout << std::endl;

	_server.run();

	std::cout << std::endl;
	std::cout << SKLL_DIM << "---------------------------------------------------------------" << SKLL_RST << std::endl;
	std::cout << std::endl;
	std::cout << SKLL_DIM << "[" << timestamp() << "]" << SKLL_RST << " "
			  << SKLL_YELLOW << "| !" << SKLL_RST << " "
			  << "Server shutting down..." << std::endl;
	std::cout << SKLL_DIM << "[" << timestamp() << "]" << SKLL_RST << " "
			  << SKLL_GREEN << "| ok" << SKLL_RST << " "
			  << "Goodbye!" << std::endl;
	std::cout << std::endl;
	return true;
}

void IRCServer::stop() {
	_server.stop();
}
