/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/21 23:14:52 by marvin            #+#    #+#             */
/*   Updated: 2025/11/21 23:14:52 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#pragma once
# include <Sockell.hpp>
# include <IRCDefs.hpp>
# include <IRCUser.hpp>
# include <map>
# include <ctime>

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   IRC SERVER - ft_irc implementation using Sockell                          */
/* ═══════════════════════════════════════════════════════════════════════════ */

class IRCServer {
public:
	IRCServer(int port, const std::string &password);
	~IRCServer();

	bool        start();
	void        stop();
	std::string hostname() const;

	static IRCServer *instance();

private:
	/* Sockell components */
	SkllServer      _server;
	SkllNetwork     _network;
	SkllProtocol    _proto;

	/* IRC state */
	int                         _port;
	std::string                 _password;
	std::string                 _host;
	std::map<fd_t, IRCUser>     _users;
	time_t                      _start_time;

	/* Static instance */
	static IRCServer *         _instance;

	/* Setup */
	void _setup_routes();

	/* Helpers */
	IRCUser *  _get_user(fd_t fd);
	fd_t        _find_by_nick(const std::string &nick);
	void        _broadcast(SkllChannel *ch, const std::string &msg, fd_t except = -1);
	void        _send_num(SkllEvent &e, const char *code, const std::string &msg);
	void        _welcome(SkllEvent &e, IRCUser *u);
	bool        _check_registered(SkllEvent &e);
	std::string _prefix(IRCUser *u);
	void        _send_names(SkllEvent &e, SkllChannel *ch);
	void        _send_banner(SkllEvent &e);

	/* Connection handlers */
	static int _on_connect(SkllEvent &e, IRCServer *s);
	static int _on_disconnect(SkllEvent &e, IRCServer *s);

	/* Registration commands */
	static int _on_pass(SkllEvent &e, IRCServer *s);
	static int _on_pass_usage(SkllEvent &e, IRCServer *s);
	static int _on_nick(SkllEvent &e, IRCServer *s);
	static int _on_nick_usage(SkllEvent &e, IRCServer *s);
	static int _on_user(SkllEvent &e, IRCServer *s);
	static int _on_user_usage(SkllEvent &e, IRCServer *s);

	/* Channel commands */
	static int _on_join(SkllEvent &e, IRCServer *s);
	static int _on_join_usage(SkllEvent &e, IRCServer *s);
	static int _on_part(SkllEvent &e, IRCServer *s);
	static int _on_part_usage(SkllEvent &e, IRCServer *s);
	static int _on_topic(SkllEvent &e, IRCServer *s);
	static int _on_topic_usage(SkllEvent &e, IRCServer *s);
	static int _on_kick(SkllEvent &e, IRCServer *s);
	static int _on_kick_usage(SkllEvent &e, IRCServer *s);
	static int _on_invite(SkllEvent &e, IRCServer *s);
	static int _on_invite_usage(SkllEvent &e, IRCServer *s);
	static int _on_mode(SkllEvent &e, IRCServer *s);
	static int _on_mode_usage(SkllEvent &e, IRCServer *s);

	/* Messaging */
	static int _on_privmsg(SkllEvent &e, IRCServer *s);
	static int _on_privmsg_usage(SkllEvent &e, IRCServer *s);
	static int _on_notice(SkllEvent &e, IRCServer *s);
	static int _on_notice_usage(SkllEvent &e, IRCServer *s);

	/* Utility */
	static int _on_ping(SkllEvent &e, IRCServer *s);
	static int _on_pong(SkllEvent &e, IRCServer *s);
	static int _on_quit(SkllEvent &e, IRCServer *s);
	static int _on_who(SkllEvent &e, IRCServer *s);
	static int _on_who_usage(SkllEvent &e, IRCServer *s);
	static int _on_list(SkllEvent &e, IRCServer *s);
	static int _on_help(SkllEvent &e, IRCServer *s);

	/* Fallback */
	static int _on_unknown(SkllEvent &e, IRCServer *s);

	IRCServer(const IRCServer &);
	IRCServer &operator=(const IRCServer &);
};