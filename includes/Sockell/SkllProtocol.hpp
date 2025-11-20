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

/*

eth0 → ethernet
wlan0 → wifi
lo → loopback
docker0 → interface Docker
tun0 → VPN
wg0 → WireGuard
rmnet → réseau mobile


struct stat st;
fstat(fd, &st);

if (S_ISSOCK(st.st_mode)) {
    // C'est un socket
} else if (S_ISREG(st.st_mode)) {
    // C'est un fichier
}

 a utiliser pour les signaux en callback pour les signaux fichier ou autres a rajouté important pour géré les signaux

TCP_NODELAY  = 1  ✓
SO_REUSEADDR = 1  ✓
O_NONBLOCK   = 1  ✓
SO_KEEPALIVE = 1  ✓
cpp// Heartbeat IRC
server.add_timer(30000, heartbeat_callback);  // 30s

// Timeout client inactif
client.add_timer(300000, disconnect_idle);    // 5min

// Rate limiting
client.add_timer(1000, reset_message_count);  // 1s

enum FDType {
    FD_SOCKET_LISTENER,
    FD_SOCKET_CLIENT,
    FD_TIMER,
    FD_SIGNAL,
    FD_EVENT,
    FD_PIPE,
    FD_USER_CUSTOM
};

std::vector<uint8_t> buffer;
buffer.resize(4096);
recv(fd, &buffer[0], buffer.size(), 0);


Protocol: - Bufferise bytes bruts
 - Cherche délimiteur(s) configuré(s)
 - Quand trouvé → callback avec bytes bruts
 - PAS de parsing, PAS de string (sauf si callback) si callback renvoie pas en brut mais renvoie un objet message dans le quelle se trouve le tableau vecteur de byte avec des fonction utile pour convertir ce vecteur en nouveau message par exemple on recois un message et dans cette class il y a les fonction send qui reprend le protocol par lequel il a été creer donc acces au server network et l'idée c'est que on puisse faire send(" msg " int "msg" bool) et du coup lui appelle le send de protocol qui lui bufferise tous ce qu'il as recu donc une string qui contient un peu tout mais toi tu t'en fout tout est mis en byte a envoyer et derriere quand ta un recv tu as ce tableau et tu sait ok donc j'ai mis msg donc je veux recuperer msg = to<std::string> puis a oui apres j'ai un int donc to<int> etc et tu pourrais avoir un msg = to() simple qui permet de récuperer une string compléte l'idée c'est que le buffer de recv est comme une stringstream ca permet de parsé les valeur donné et tout je sais pas si tu te souviens de notre conversation avec la console ou on avais reussi a faire console.log() << "hey" << var << bool; ca peux etre ultra pratique de construire sont message comme ca

message get network pour send si besoin message contient le tableau de byte complet sans le $end

erreurs //  451

 */

enum e_SkllProtocolConnexion
{
	ETERNET, // eth0
	WIFI, // wlan0
	LOOPBACK, // lo
	DOCKER, // docker0
	VPN, // tun0
	WIREGUARD, // wg0
	MOBILE, // rmnet
}

enum SkllProtocolOpt {
    SKLL_REUSEADDR      = 1 << 0,
    SKLL_REUSEPORT     = 1 << 1,
    SKLL_SENDBUF   = 1 << 2,
    SKLL_RECVBUF    = 1 << 3,
    SKLL_LINGER = 1 << 4,
    SKLL_TIMEOUT_SEND      = 1 << 5,
    SKLL_TIMEOUT_RECV    = 1 << 6,
    SKLL_BUFFER       = 1 << 7,
    SKLL_CHUNK       = 1 << 8

};

struct SkllProtocolOptions {
    bool reuse_addr = true;
    bool reuse_port = false;
    bool send_buf = false;
    bool recv_buf = false;
    bool linger = false;
    int send_timeout = 0;
    int recv_timeout = 0;
	int buffer = 512;
	int	chunk = 16;
};
// possible pendant l'execution 
struct SkllProtocolTCPOptions {
    bool nodelay       = true;
    bool cork          = false;   // remplace quicKARK
    bool quickack      = false;
    int  keepalive_time   = 7200;
    int  keepalive_intvl  = 75;
    int  keepalive_probes = 9;
};

 
struct SkllProtocolAddrs
{
	sockaddr_storage local_addr;
	sockaddr_storage remote_addr;
	socklen_t        local_len;
	socklen_t        remote_len;
};



class SkllProtocol {
	private:
		int			_fd;
		std::string	_name;
		int			_opts;
		bool		_running;

		std::map<std::string, SkllClient*>	_clients;
		std::map<int, SkllClient*>			_clients_tcp;

		SkllHook							_hook;
	public:

		~SkllProtocol();
		SkllProtocol();
		SkllProtocol(const std::string &name, const std::string &addr, int port, int opts);
enum SkllEvent {
    ON_START      = 1 << 0,
    ON_UPDATE     = 1 << 1,
    ON_SHUTDOWN   = 1 << 2,
    ON_CONNECT    = 1 << 3,
    ON_DISCONNECT = 1 << 4,
    ON_ERROR      = 1 << 5,
    ON_TIMEOUT    = 1 << 6,
    ON_RECV       = 1 << 7,
    ON_SEND       = 1 << 8
};


		// bindind finial on peux plus toucher a socket
		int		on_start();
		int		on_update();
		int		on_stop();
		int		on_shutdowm();
		int		on_connect();
		int		on_disconnect();
		int		on_error();
		int		on_recv();
		int		on_send();
		
        

		void	send(int fd, const char *data, size_t len);
		void	broadcast(const char *data, size_t len);

		SkllProtocol	&set_reuseaddr(bool e = true);
		SkllProtocol	&set_nodelay(bool e = true);
		SkllProtocol	&set_quickack(bool e = true);
		SkllProtocol	&on(int event, SkllHook::Callback cb, void *user_data = NULL);


	private:
		SkllClient	&client(SkllClient &client);
		
		SkllProtocol(const SkllProtocol &);
		SkllProtocol	&operator=(const SkllProtocol &);
};
