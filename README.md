
[:prefix] COMMAND [params] :trailing\r\n

:Nicolas!~nick@host PRIVMSG #general :Hello world\r\n
JOIN #general\r\n
MODE #general +k 123\r\n


socket(AF_INET | AF_INET6 | AF_UNIX, TCP | UDP, auto | TCP | UDP)
int fd = socket(AF_INET, SOCK_STREAM, 0);  // TCP
int fd_udp = socket(AF_INET, SOCK_DGRAM, 0);  // UDP

ipv4 ipv6 local

protocol.on(SKLL_IPV4, SKLL_TCP, "0.0.0.0", 6667)
		.set_crlf("\r\n")
		.set_binary(false)
		.set_size(512);
protocol.on(SKLL_IPV4 | SKLL_IPV6, SKLL_TCP | UDP , "0.0.0.0", 8080);
protocol.on(SKLL_IPV6, SKLL_UDP, "0.0.0.0", 7777);


int err = getaddrinfo(host, port, &hints, &res); //throw
EAI_AGAIN	Résolution temporairement impossible (DNS down)
EAI_FAIL	Échec permanent (ex: DNS invalide)
EAI_NONAME	Host ou service introuvable
EAI_MEMORY	Pas assez de mémoire
EAI_FAMILY	Famille d’adresse non supportée (IPv6, etc.)


┌─────────────────────────────────────────────────────────┐
│                     SkllServer                          │
│  • Gère FD limits                                       │
│  • Coordonne Networks                                   │
│  • Broadcast global                                     │
└────────────┬────────────────────────────────────────────┘
             │
             ├─── SkllNetwork #1 (IRC)
             │    │
             │    ├─── SkllProtocol (TCP:6667)
             │    │    └─── Clients FD [5, 6, 7...]
             │    │
             │    └─── SkllProtocol (UDP:9000)
             │         └─── Clients IP:PORT
             │
             ├─── SkllNetwork #2 (Web)
             │    └─── SkllProtocol (TCP:8080)
             │
             └─── SkllChannel (conteneurs logiques)
                  ├─── Channel #general
                  │    └─── Client IDs [alice, bob...]
                  └─── Channel #game
                       └─── Client IDs [player1, player2...]
