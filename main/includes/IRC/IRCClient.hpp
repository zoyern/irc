/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCClient.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/21 23:13:37 by marvin            #+#    #+#             */
/*   Updated: 2025/11/21 23:13:37 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <Sockell/SkllClient.hpp>
#include <Sockell/SkllHook.hpp>
#include <string>
#include <set>

class IRCClient {
public:
	SkllClient *skll_client;
	SkllHook hook;
	
	std::string nickname;
	std::string username;
	std::string realname;
	std::string hostname;
	
	bool authenticated;
	bool registered;
	int password_attempts;
	
	std::set<std::string> channels;
	
	IRCClient();
	IRCClient(SkllClient *client);
	~IRCClient();
	
	IRCClient &on(int event, SkllHook::Callback cb, void *user_data = NULL);
	
	void send(const std::string &msg);
	void send_numeric(const std::string &code, const std::string &msg);
	void send_error(const std::string &code, const std::string &msg);
	
	bool is_authenticated() const;
	bool is_registered() const;
	bool is_in_channel(const std::string &channel) const;
	
	void join_channel(const std::string &channel);
	void part_channel(const std::string &channel);
	
	std::string get_prefix() const;
	
	int get_fd() const;
	const std::string &get_id() const;
	
	static void on_auth(void *event, void *user_data);
	static void on_register(void *event, void *user_data);
	static void on_message(void *event, void *user_data);
};