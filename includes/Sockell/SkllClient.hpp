/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IrcClient.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 22:36:20 by marvin            #+#    #+#             */
/*   Updated: 2025/11/03 22:36:20 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <Sockell/SkllHook.hpp>
#include <Sockell/SkllMessage.hpp>
#include <string>
#include <vector>
#include <deque>
#include <ctime>
#include <stdint.h>


struct SkllRateLimit
{
	size_t	max_messages;
	time_t	window_seconds;

	std::deque<time_t>	timestamps;
	
	SkllRateLimit();
	SkllRateLimit(size_t max_msg, time_t window);
	
	bool	is_allowed();
	void	record();
	void	reset();
};

class SkllClient {
public:
	int			fd;
	std::string	id;
	SkllHook	hook;
	void		*userdata;
	
	SkllMessage	recv_msg;
	SkllMessage	send_msg;
	
	std::vector<uint8_t>	buffer;
	SkllRateLimit			rate_limit;
	
	~SkllClient();
	SkllClient();
	SkllClient(int client_fd, const std::string &client_id);
	
	SkllClient	&on(int event, SkllHook::Callback cb, void *user_data = NULL);
	
	void		send(const std::string &data);
	void		send(SkllMessage &msg);
	
	void		set_fd(int client_fd);
	void		set_id(const std::string &client_id);
	void		set_rate_limit(size_t max_messages, time_t window_seconds);
	bool		is_rate_limited();
	
	int					get_fd() const;
	const std::string	&get_id() const;
	
private:
	SkllClient(const SkllClient&);
	SkllClient &operator=(const SkllClient&);
};
