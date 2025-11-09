
[:prefix] COMMAND [params] :trailing\r\n

:Nicolas!~nick@host PRIVMSG #general :Hello world\r\n
JOIN #general\r\n
MODE #general +k 123\r\n

```cpp
MAIN
│
├─ Vérification arguments
│      argc != 3 → affiche usage → exit
│
├─ Initialisation serveur
│      SkllServer srv(argv[1], argv[2])
│
├─ (1) Configuration générale
│      srv.name("Sockell")
│         .address("0.0.0.0")
│         .connexion_msg("Welcome!")
│         .max_clients(100)
│         .reserved_fds(10)
│         .timeout(30)
│         .queue(128)
│         .protocols_supported(TCP | UDP | WebSocket | HTTP)
│
├─ (2) Enregistrement des hooks serveur
│      srv.hook(ON_START, &on_start, NULL)
│         .hook(ON_UPDATE, &on_update, NULL)
│         .hook(ON_SHUTDOWN, &on_shutdown, NULL)
│         .hook(ON_CONNECT, &on_connect, NULL)
│         .hook(ON_DISCONNECT, &on_disconnect, NULL)
│         .hook(ON_ERROR, &on_error, NULL)           ← gestion erreurs
│         .hook(ON_TIMEOUT, &on_timeout, NULL)
│         .hook(ON_RECV, &on_recv, NULL)
│         .hook(ON_SEND, &on_send, NULL)
│
├─ (3) Configuration protocole IRC / custom
│      srv.protocol("irc", "\r\n", false, TCP)
│         .buffer_size(2048)
│         .on("NICK", handle_nick)
│         .on("JOIN", handle_join)
│         .on("PART", handle_part)
│         .on("PRIVMSG", handle_privmsg)
│         .on("MODE", handle_mode)
│
├─ (4) Configuration des channels par défaut
│      srv.channel(CHANNEL_NAME, CHANNEL_DEFAULT)
│         .password("")
│         .op(CHANNEL_OPERATOR)
│         .size(CHANNEL_SIZE)
│         .invite_only(CHANNEL_INV_ONLY)
│         .restricted_topic(CHANNEL_REST_TOPIC)
│         .topic(CHANNEL_TOPIC)
│         │
│         ├─ Hooks channel
│         │      ON_START       → &on_start
│         │      ON_UPDATE      → &on_update
│         │      ON_SHUTDOWN    → &on_shutdown
│         │      ON_CONNECT     → &on_connect
│         │      ON_DISCONNECT  → &on_disconnect
│         │      ON_ERROR       → &on_error       ← gestion erreurs par channel
│         │      ON_TIMEOUT     → &on_timeout
│         │      ON_RECV        → &on_recv
│         │      ON_SEND        → &on_send
│         │      ON_MODE_CHANGE → handle_mode
│         │      ON_JOIN        → handle_join
│         │      ON_LEAVE       → handle_part
│         │
│         └─ Commandes IRC / custom protocol
│                NICK, JOIN, PART, PRIVMSG, MODE, DECONNECT, GAME_CMD
│
└─ (5) Lancement du serveur
       return srv.run()

─────────────────────────────────────────────────────────────
SkllServer::run() — Flow détaillé
─────────────────────────────────────────────────────────────

SkllServer::run()
│
├─ Network::setup(protocol)
│      - Création socket TCP / UDP / WebSocket / HTTP
│      - bind(), listen()
│      - epoll_create()
│
├─ HookManager::trigger(ON_START)
│
├─ Boucle principale epoll / select / poll
│      │
│      ├─ Nouveaux clients
│      │      Network::accept() → Client*
│      │      ClientManager::add(Client*)
│      │      HookManager::trigger(ON_CONNECT)
│      │
│      ├─ Données reçues
│      │      Network::recv(Client*) → string
│      │      Protocol::parse(string) → Command/Message
│      │      SkllServer::handle_message(Client*, Message)
│      │      ChannelManager / Command handlers
│      │      HookManager::trigger(ON_RECV)
│      │
│      ├─ Erreurs détectées
│      │      HookManager::trigger(ON_ERROR, code)
│      │      console.error(...)
│      │
│      └─ Déconnexion
│             ClientManager::remove(Client*)
│             HookManager::trigger(ON_DISCONNECT)
│
└─ Timeout / Stop signal
       HookManager::trigger(ON_SHUTDOWN)
       Cleanup resources

─────────────────────────────────────────────────────────────
CLASSES & HOOKS (SkllServer-side)
─────────────────────────────────────────────────────────────

SkllServer
│
├─ Propriétés :
│     - address, port, name
│     - max_clients, timeout
│     - listen_fd, epoll_fd
│     - clients map<fd, Client>
│     - channels map<name, Channel>
│     - protocols_supported (TCP | UDP | WS | HTTP)
│     - console
│     - HookManager hooks
│
├─ Fonctions principales :
│     - run(), stop()
│     - handle_message(Client*, Message&)
│     - broadcast(Message&, Channel* optional)
│     - channel(name, default=false)
│     - protocol(name, delimiter, uppercase, type)
│     - hook(e_hook, callback, void* data)
│
└─ Hooks disponibles :
      ON_START, ON_UPDATE, ON_SHUTDOWN, ON_CONNECT, ON_DISCONNECT,
      ON_ERROR, ON_TIMEOUT, ON_SEND, ON_RECV

Channel
│
├─ Propriétés :
│     - name, password
│     - operators list<string>
│     - max_size, invite_only, topic
│     - clients map<fd, Client>
│     - HookManager hooks
│
├─ Fonctions principales :
│     - join(Client*), leave(Client*)
│     - kick(Client*)
│     - broadcast(Message&)
│     - set_mode(char, optional value)
│     - hook(e_hook, callback, void* data)
│
└─ Hooks disponibles :
      ON_START, ON_UPDATE, ON_SHUTDOWN, ON_CONNECT, ON_DISCONNECT,
      ON_ERROR, ON_TIMEOUT, ON_SEND, ON_RECV, ON_MODE_CHANGE,
      ON_JOIN, ON_LEAVE

Client
│
├─ Propriétés :
│     - fd, nick, user, host
│     - current_channel
│     - input_buffer, output_buffer
│
├─ Fonctions principales :
│     - send(Message&)
│     - receive()
│     - join_channel(Channel*)
│     - leave_channel()
│     - set_nick(string), set_user(string)

HookManager / Hook
│
├─ Propriétés :
│     - bitmask (e_hook)
│     - vector<callback>
│
└─ Fonctions :
      - register(callback)
      - trigger(args...)

Network
│
├─ Propriétés :
│     - epoll_fd
│     - TCP / UDP / WebSocket / HTTP sockets
│
└─ Fonctions :
      - setup(protocol)
      - accept() → Client*
      - send(Client*, string)
      - recv(Client*) → string
      - handle_errors(code)

Protocol
│
├─ Fonctions :
│     - parse_message(string) → Command/Message
│     - format_message(Command)
│     - handle_file_transfer(Client*, string)
│     - validate(string)
│     - handle_game_command(Client*, string)

Console
│
├─ Fonctions :
│     - log(string, level)
│     - info(string)
│     - debug(string)
│     - warn(string)
│     - error(string)
│     - clear()
```