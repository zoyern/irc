/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Lobby.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 22:37:00 by marvin            #+#    #+#             */
/*   Updated: 2025/11/03 22:37:00 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <Sockell/SkllHook.hpp>
#include <string>
#include <map>
#include <cstddef>

class SkllClient;

class SkllChannel {
	public:
		std::string	name;
		SkllHook	hook;
		void		*userdata;

		std::map<int, SkllClient*>	clients;
		
		~SkllChannel();
		SkllChannel();
		SkllChannel(const std::string &n);

		void broadcast(const std::string &msg, int exclude_fd);
		
		SkllChannel	&on(int event, SkllHook::Callback cb, void *user_data = NULL);
		
		void		add_client(int fd, SkllClient *client);
		void		remove_client(int fd);
		SkllClient	*get_client(int fd);
		bool		has_client(int fd) const;
		size_t		client_count() const;

	private:
		SkllChannel(const SkllChannel&);
		SkllChannel	&operator=(const SkllChannel&);
};
