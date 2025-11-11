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

enum e_skllprotocol {
	SKLL_TCP	= 1 << 0, // SOCK_STREAM 
	SKLL_UDP	= 1 << 1, // SOCK_DGRAM
};

enum e_skllips {
	SKLL_IPV4	= 1 << 0, // AF_INET
	SKLL_IPV6	= 1 << 1, //AF_INET6 
};

// Énumération pour les options de socket communes
enum SkllSocketOption {
    // Options communes à tous les sockets (niveau SOL_SOCKET)
    OPT_REUSE_ADDRESS,         // Permet de réutiliser une adresse locale même si elle est en TIME_WAIT
    OPT_REUSE_PORT,            // Permet à plusieurs sockets de partager le même port (utile pour le load balancing)
    OPT_KEEPALIVE,             // Envoie périodiquement des paquets pour vérifier si la connexion est active
    OPT_BROADCAST,             // Autorise l'envoi de paquets broadcast (UDP)
    OPT_RECEIVE_BUFFER_SIZE,   // Définit la taille du buffer de réception
    OPT_SEND_BUFFER_SIZE,      // Définit la taille du buffer d'envoi
    OPT_RECEIVE_TIMEOUT,       // Définit un timeout pour les opérations de réception
    OPT_SEND_TIMEOUT,          // Définit un timeout pour les opérations d'envoi
    OPT_LINGER,                // Contrôle le comportement de close() quand des données sont en attente

};


class SkllProtocol {
	private:
		int			_fd;
		std::string _crlf;
		bool		_binary;
		int			_transport;
		int			_buffer_size;

		/*SkllTCP		_tcp;
		SkllUDP		_udp;*/

		//hooks
	public:
		~SkllProtocol();
		SkllProtocol();

	public:
		SkllProtocol	&set_reuse_addr(bool opt = true);
		SkllProtocol	&set_reuse_port(bool opt = true);
		SkllProtocol	&set_recv_buf(int size);
		SkllProtocol	&set_send_buf(int size);
		SkllProtocol	&set_recv_timeout(bool opt = true);
		SkllProtocol	&set_send_timeout(bool opt = true);
		SkllProtocol	&set_keepalive(bool opt = true);
		SkllProtocol	&set_linger(bool opt = true);
		SkllProtocol	&set_nodelay(bool opt = true);
		SkllProtocol	&set_cork(bool opt = true);
		SkllProtocol	&set_quick_arks(bool opt = true);
		SkllProtocol	&set_ipv6_only(bool opt = true);

};
