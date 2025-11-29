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
#include <Sockell/SkllTypes.hpp>
#include <Sockell/SkllBuffer.hpp>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
/* ═══════════════════════════════════════════════════════════════════════════ */
/*   CLIENT - Connected socket with I/O buffers                                */
/*   • Template data() for typed access without void* cast                     */
/*   • Input/output buffers                                                    */
/*   • Copyable for std::map storage (no heap allocation)                      */
/* ═══════════════════════════════════════════════════════════════════════════ */

class SkllClient
{
	private:
		fd_t					_fd;
		int						_port;
		void					*_data;
		SkllBuffer				_in;
		SkllBuffer				_out;
		std::string				_ip;
		
		bool					_is_udp;
		socklen_t				_udp_addr_len;
		struct sockaddr_storage	_udp_addr;
	public:
		~SkllClient();
		SkllClient();
		SkllClient(const SkllClient &other);
		SkllClient	&operator=(const SkllClient &other);

		void						reset();
		void						init(fd_t fd);
		void						set_raw_data(void *ptr);
		void						set_addr(const std::string &ip, int port);
		void						set_udp_addr(const struct sockaddr_storage &addr, socklen_t len);
		SkllBuffer					&in();
		SkllBuffer					&out();
		void						*raw_data();
		template <typename T> T		*data();
		template <typename T> void	set_data(T *ptr);

		fd_t							fd() const;
		int								port() const;
		bool							is_udp() const;
		bool							active() const;
		socklen_t						udp_addr_len() const;
		const SkllBuffer				&in() const;
		const SkllBuffer				&out() const;
		const std::string				&ip() const;
		const struct sockaddr			*udp_addr() const;
		const void						*raw_data() const;
		template <typename T> const T	*data() const;
};

#include <Sockell/SkllClient/SkllClient.tpp>
