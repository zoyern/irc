/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SkllNetwork.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/09 16:27:41 by marvin            #+#    #+#             */
/*   Updated: 2025/11/09 16:27:41 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <Sockell/SkllProtocol.hpp>
#include <sys/epoll.h>
#include <vector>
#include <map>

class SkllServer;

class SkllNetwork {
	private:
		std::string	_name;
		int			_epfd;
		int			_timeout;
		int			_queue;
		uint32_t	_events;
		SkllServer	*_server;

		std::vector<epoll_event>				_ev;
		std::map<std::string, SkllProtocol*>	_protos;
		std::map<int, SkllProtocol*>			_fd_proto;

		std::map<std::string, SkllProtocol*>	_protocols;
		std::map<int, SkllProtocol*>			_protocols_fd;
	public:
		SkllHook	hook;

		~SkllNetwork();
		SkllNetwork();
		SkllNetwork(const std::string &name, int timeout, int queue);
		SkllNetwork(const SkllNetwork &other);
		SkllNetwork	&operator=(const SkllNetwork &other);
		
		void			run();
		int		start();
		int		wait();
		int		stop();
		int		shutdown();
		void			update();
		void			set_server(SkllServer *s);
		void			add_event(int fd);
		void			destroy_event(int fd);
		SkllProtocol	&listen(const SkllProtocol &protocol);
		void			broadcast(const char *data, size_t len);
		SkllNetwork		&on(int event, SkllHook::Callback cb, void *user_data = NULL);
		
		SkllProtocol	*get_protocol(const std::string &name);

		int				get_epfd() const;
		size_t			protocol_count() const;
		std::string		get_name() const;
		std::string		print_protocols() const;

	private:
		void		_handle_listen(int fd, uint32_t ev);
		void		_handle_client(int fd, uint32_t ev);
		void		_accept(int fd);
		void		_recv(int fd);
		std::string	_err_epoll();

	public:
		//set_wakeup(bool opt = false);

		std::string	name() const;
		int	fd() const;
		
};