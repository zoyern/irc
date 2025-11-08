/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Epoll.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/08 00:45:11 by marvin            #+#    #+#             */
/*   Updated: 2025/11/08 00:45:11 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <irc.hpp>

#define MAX_EVENTS 128

class Epoll {
	private:
		int                 _epfd;
		struct epoll_event  _events[MAX_EVENTS];

	public:
		Epoll();
		~Epoll();

		void    add(int fd, uint32_t events);
		void    del(int fd);
		int     wait(int timeout);
		struct epoll_event* events();
		int     fd() const;
};