#include "Irc.hpp"

Server::Server(void) : _password(""), _name("irc")
{}
Server::Server(std::string password) : _password(password)
{};

Server::Server(const std::string password) : _password(password), _name("irc") {

    _hook.on(SKLL_ON_START, on_server_start, this);
    _hook.on(SKLL_ON_CONNECT, on_client_connect, this);
    _hook.on(SKLL_ON_DISCONNECT, on_client_disconnect, this);

    setup_default_channels();
}

bool Server::checkPassword(std::string pass)
{
	return _password == pass;
}

const std::vector<Channel> &Server::getChannels() const
{
	return _channels;
}

Channel *Server::getOneChannel(std::string name)
{
	std::vector<Channel>::iterator it = _channels.begin();
	std::vector<Channel>::iterator ite = _channels.end();
	while(it != ite)
	{
		if(it->getName() == name)
			return &(*it);
		it++;
	}
	return NULL;
}
void Server::addChannel(Channel &channel)
{
	_channels.push_back(channel);
}
void Server::deleteChannel(std::string name)
{
	std::vector<Channel>::iterator it = _channels.begin();
	std::vector<Channel>::iterator ite = _channels.end();
	while(it != ite)
	{
		if(it->getName() == name)
			_channels.erase(it);
		it++;
	}
}

Client *Server::getOneClient(std::string nick)
{
	std::vector<Client>::iterator it = _clients.begin();
	std::vector<Client>::iterator ite = _clients.end();
	while(it != ite)
	{
		if(it->getNick() == nick)
			return &(*it);
		it++;
	}
	return NULL;
}
Client *Server::getOneClient(int  fd)
{
	std::vector<Client>::iterator it = _clients.begin();
	std::vector<Client>::iterator ite = _clients.end();
	while(it != ite)
	{
		if(it->getFd() == fd)
			return &(*it);
		it++;
	}
	return NULL;
}

const std::vector<Client> &Server::getClients() const
{
	return _clients;
}

void Server::addClient(Client &client)
{
	_clients.push_back(client);
}
void Server::deleteClient(int fd)
{
	std::vector<Client>::iterator it = _clients.begin();
	std::vector<Client>::iterator ite = _clients.end();
	while(it != ite)
	{
		if(it->getFd() == fd)
			_clients.erase(it);
		it++;
	}
}

void Server::on_server_start(void *event, void *user_data) {
	SkllEvent *evt = (SkllEvent*)event;
	Server *irc = (Server*)user_data;

	if (!evt->server) return;

	SkllServer *server = evt->server;

	std::cout << "\n"
			  << SKLL_CYAN << "╔════════════════════════════════════════╗\n"
			  << "║ " << SKLL_BOLD << "IRC SERVER" << SKLL_RESET << SKLL_CYAN
			  << "                             \n"
			  << "╠════════════════════════════════════════\n"
			  << "║ " << SKLL_RESET << "Max Clients:  "
			  << SKLL_BOLD << std::setw(4) << server->count() << SKLL_RESET << SKLL_CYAN
			  << "                      \n"
			  << "║ " << SKLL_RESET << "FD Reserved:  "
			  << SKLL_BOLD << std::setw(4) << server->get_fd_limit() << SKLL_RESET << SKLL_CYAN
			  << "                      \n"
			  << "║ " << SKLL_RESET << "Password:     "
			  << SKLL_DIM << (irc->_password.empty() ? "none" : "***")
			  << SKLL_RESET << SKLL_CYAN << "                       \n"
			  << "╚════════════════════════════════════════╝"
			  << SKLL_RESET << "\n" << std::endl;
}

void Server::on_client_connect(void *event, void *user_data) {
	SkllEvent *evt = (SkllEvent*)event;
	Server *irc = (Server*)user_data;

	if (!evt->client) return;

	evt->client->set_rate_limit(10, 1);

	Client *client = new Client(evt->client);
	evt->client->userdata = client;

	irc->_clients[evt->client->get_fd()] = client;

	client->send(IRCErrors::welcome_banner());

	std::cout << SKLL_GREEN << "[CONNECT] " << SKLL_RESET
			  << SKLL_BOLD << evt->client->get_id() << SKLL_RESET
			  << " (fd=" << evt->client->get_fd() << ")" << std::endl;
}

void Server::on_client_disconnect(void *event, void *user_data) {
	SkllEvent *evt = (SkllEvent*)event;
	Server *irc = (Server*)user_data;

	if (!evt->client) return;

	Client *client = (Client*)evt->client->userdata;
	if (client) {
		std::ostringstream quit_msg;
		quit_msg << ":" << client->nickname << "!" << client->username << "@" << client->hostname
				 << " QUIT :Connection lost\r\n";

		std::set<std::string>::iterator it;
		for (it = client->channels.begin(); it != client->channels.end(); ++it) {
			IRCChannel *channel = irc->get_channel(*it);
			if (channel) {
				channel->broadcast(quit_msg.str(), client->get_fd());
			}
		}

		std::cout << SKLL_RED << "[DISCONNECT] " << SKLL_RESET;

		if (!client->nickname.empty()) {
			std::cout << SKLL_BOLD << client->nickname << SKLL_RESET << " ";
		}

		std::cout << "(" << evt->client->get_id() << ")" << std::endl;
	}

	irc->remove_client(evt->client->get_fd());
}
Server::~Server()
{}
