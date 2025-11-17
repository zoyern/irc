/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SkllProtocol.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/08 21:40:23 by marvin            #+#    #+#             */
/*   Updated: 2025/11/08 21:40:23 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <Sockell.hpp>
#include <Sockell/SkllHook.hpp>
#include <Sockell/SkllErrors.hpp>
#include <Sockell/SkllClient.hpp>
#include <string>
#include <map>

class SkllProtocol {
	private:
		int			_fd;
		std::string	_name;
		std::string	_addr;
		int			_port;
		int			_opts;
		bool		_bound;

		std::map<int, SkllClient*>	_clients;
	public:
		SkllHook	hook;

		~SkllProtocol();
		SkllProtocol();
		SkllProtocol(const SkllProtocol &other);
		SkllProtocol	&operator=(const SkllProtocol &other);

		void	run();
		void	create(const std::string &name, const std::string &addr, int port, int opts);
		void	add_client(int fd, SkllClient *c);
		void	remove_client(int fd);
		void	send(int fd, const char *data, size_t len);
		void	broadcast(const char *data, size_t len);

		const std::map<int, SkllClient*>	&get_clients() const;

		SkllProtocol	&set_reuseaddr(bool e = true);
		SkllProtocol	&set_nodelay(bool e = true);
		SkllProtocol	&set_quickack(bool e = true);
		SkllProtocol	&set_keepalive(bool e = true);
		SkllProtocol	&on(int event, SkllHook::Callback cb, void *user_data = NULL);
		
		SkllClient	*get_client(int fd);
		std::string	get_address() const;
		std::string	get_name() const;
		bool		is_tcp() const;
		bool		is_bound() const;
		int			get_fd() const;
		int			get_port() const;

	private:
		std::string	_err_socket();
		std::string	_err_bind();
		std::string	_err_listen();
};