/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/21 23:15:01 by marvin            #+#    #+#             */
/*   Updated: 2025/11/21 23:15:01 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include <IRC.hpp>

IRCServer::~IRCServer() { clean(); std::cout << "destruct" << std::endl; }
IRCServer::IRCServer(int port, const std::string &password) 
	: _tcpfd(-1)
	, _epfd(-1)
	, _password(password)
	, _port(port)
	, _adress("0.0.0.0")
	{ std::cout << "constructor" << std::endl; }
int	IRCServer::stop() {return (clean(), std::cout << "stop" << std::endl, 0);}

int	IRCServer::run() {
	_socket_create();
	_epoll_create();
	_handle_wait();
	std::cout << "run" << std::endl;
	return (0);
}


int	IRCServer::clean() {
	if (_tcpfd > -1) ::close(_tcpfd);
	if (_epfd > -1) ::close(_epfd);
	std::cout << "clean" << std::endl;
	return (0);
}

int	IRCServer::_socket_create()
{
	_tcpfd = socket(AF_INET, SOCK_STREAM, 0);
	if (_tcpfd < 0) throw (std::runtime_error("Failed to create socket."));
	int	opt = 1;
	struct sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(_port);
	serverAddress.sin_addr.s_addr = inet_addr("0.0.0.0");;
	if (setsockopt(_tcpfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) throw (std::runtime_error("Failed setsockopt."));
	if (fcntl(_tcpfd, F_SETFL, O_NONBLOCK) < 0) throw (std::runtime_error("Failed fcntl."));
	if (bind(_tcpfd, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1) throw (std::runtime_error("Failed to bind socket."));
	if (listen(_tcpfd, SOMAXCONN) < 0) throw (std::runtime_error("Failed to listen."));
	std::cout << "TCP IS LISTEN ON : [ 0.0.0.0:" << _port << " ] !" << std::endl;
	return (0);
}

int	IRCServer::_epoll_create()
{
	_epfd = epoll_create1(0);
	if (_epfd < 0) throw (std::runtime_error("Failed to create epollFd."));
	_epoll_add(_tcpfd);
	std::cout << "EPOLL IS LISTEN TCP ON : [ fd:" << _tcpfd << " ] !" << std::endl;
	return (0);
}

void	IRCServer::_epoll_add(fd_t fd)
{
	struct epoll_event event;
	event.events = EPOLLIN;
	event.data.fd = fd;
	if (epoll_ctl(_epfd, EPOLL_CTL_ADD, fd, &event) < 0) throw (std::runtime_error("Failed to add fd to epoll instance."));
}

int	IRCServer::_handle_wait()
{
	int	len;
	struct epoll_event events[128];
	while (true)
	{
		len = epoll_wait(_epfd, events, 128, 0);
		if (len < 0) std::cout << "Failed to get events." << std::endl;
		for (int i = 0; i < len; i++)
			_handle_root(events[i]);
		
	}
	return (0);
}

int	IRCServer::_handle_root(struct epoll_event &e)
{
	if (e.data.fd == _tcpfd)
		_handle_connect();
	else if (e.events &(EPOLLHUP))
		_handle_disconnect(e);
	else if (e.events &(EPOLLERR)) 
		_handle_error(e);
	 else if (e.events &EPOLLIN) 
		_handle_message(e);
	return (0);
}

int     IRCServer::_handle_connect()
{
	struct sockaddr_in clientAddress;
	socklen_t clientAddressLength = sizeof(clientAddress);
	int fd = accept(_tcpfd, (struct sockaddr*)&clientAddress, &clientAddressLength);
	if (fd < 0) std::cerr << "Failed to accept client connection." << std::endl;
	_epoll_add(fd);
	std::cout << "CLIENT : " << fd << " _handle_connect" << std::endl;
	return (0);
}
int     IRCServer::_handle_message(struct epoll_event &e)
{
	std::cout << "CLIENT : " << e.data.fd << " _handle_message" << std::endl;
	return (0);
}
int     IRCServer::_handle_error(struct epoll_event &e)
{
	std::cout << "CLIENT : " << e.data.fd << " _handle_error" << std::endl;
	return (0);
}
int     IRCServer::_handle_disconnect(struct epoll_event &e)
{
	std::cout << "CLIENT : " << e.data.fd << " _handle_disconnect" << std::endl;
	return (0);
}

bool Server::_signal = false;
void Server::setSignal(bool b)
{
	_signal = b;
}

Server::Server(int port, std::string password) : _port(port), _password(password), _serverFd(-1), _epollFd(-1)
{
}

void Server::_setSocket(void)
{
	int opt = 1;
	_serverFd = socket(AF_INET, SOCK_STREAM, 0);
	if (_serverFd == -1)
	{
		std::cerr << "Failed to create socket." << std::endl;
		return;
	}
	_serverAddress.sin_family = AF_INET;
	_serverAddress.sin_port = htons(_port);
	_serverAddress.sin_addr.s_addr = INADDR_ANY;
	if (setsockopt(_serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
	{
		std::cerr << "Failed setsockopt." << std::endl;
		return;
	}

	if (fcntl(_serverFd, F_SETFL, O_NONBLOCK) == -1)
	{
		std::cerr << "Failed fcntl." << std::endl;
		return;
	}

	if (bind(_serverFd, (struct sockaddr *)&_serverAddress, sizeof(_serverAddress)) == -1)
	{
		std::cerr << "Failed to bind socket." << std::endl;
		return;
	}

	if (listen(_serverFd, SOMAXCONN) == -1)
	{
		std::cerr << "Failed to listen." << std::endl;
		return;
	}

	_event.events = EPOLLIN;
	_event.data.fd = _serverFd;
	_epollFd = epoll_create1(0);

	if (_epollFd == -1)
	{
		std::cerr << "Failed to create epollFd." << std::endl;
		return;
	}
	if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, _serverFd, &_event) == -1)
	{
		std::cerr << "Failed to add server socket to epoll instance." << std::endl;
		return;
	}
	std::cout << "Server is running on port " << _port << std::endl;
}

void Server::_handleNewClient(void)
{

	IRCUser newClient;
	memset(&_clientAddress, 0, sizeof(_clientAddress));
	socklen_t clientAddressLength = sizeof(_clientAddress);

	int clientFd = accept(_serverFd, (struct sockaddr *)&_clientAddress, &clientAddressLength);
	newClient.setFd(clientFd);
	if (clientFd == -1)
	{
		std::cerr << "Failed to accept client connection." << std::endl;
		return;
	}
	_event.events = EPOLLIN;
	_event.data.fd = clientFd;
	if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, clientFd, &_event) == -1)
	{
		std::cerr << "Failed to add client socket to epoll instance." << std::endl;
		close(clientFd);
		return;
	}
	_clients.push_back(newClient);
}

void Server::cmd_pass(std::vector<std::string> cmd, IRCUser *client)
{
	(void)cmd;
	(void)client;
}

void Server::_handleCmd(std::string str, IRCUser *client)
{
	std::vector<std::string> cmd = splitCmd(str);
	size_t found = str.find_first_not_of(" \t\v");
	if (found != std::string::npos)
		str = str.substr(found);
	if(str.length() < 1)
		return;
	if (str[0] != '/')
		client->send("Command must begin with /, ex: /JOIN or /PASS\n");
	if(cmd[0] == "/pass")
		cmd_pass(cmd, client);
	// if(cmd[0] == "/user")
	// if(cmd[0] == "/nick")
	// if(cmd[0] == "/join")
	// if(cmd[0] == "/invite")
	// if(cmd[0] == "/kick")
	// if(cmd[0] == "/mode")
	// if(cmd[0] == "/topic")
	// if(cmd[0] == "/quit")
}

void Server::_handleClientData(int fd)
{
	IRCUser *client = getOneClient(fd);
	std::vector<std::string> cmd;
	char buff[1024];
	memset(buff, 0, sizeof(buff));
	ssize_t bytes = recv(fd, buff, sizeof(buff) - 1, 0);
	if (bytes <= 0)
		return;
	else
	{
		client->setBuffer(buff);
		if (client->getBuffer().find_first_not_of("\r\n") == std::string::npos)
			return;
		cmd = splitCmdLine(client->getBuffer());
		for (size_t i = 0; i < cmd.size(); i++)
			_handleCmd(cmd[i], client);
		if(getOneClient(fd))
			client->clearBuffer();
	}
}

void Server::initServer()
{
	_setSocket();
	while (Server::_signal == false)
	{
		int numEvents = epoll_wait(_epollFd, _events, 128, 0);
		if (numEvents == -1)
		{
			std::cerr << "Failed to wait for events." << std::endl;
			break;
		}
		for (int i = 0; i < numEvents; ++i)
		{
			if (_events[i].data.fd == _serverFd)
				_handleNewClient();
			else
				_handleClientData(_events[i].data.fd);
		}
	}
}

const std::vector<IRCChannel> &Server::getChannels() const
{
	return _channels;
}

IRCChannel *Server::getOneChannel(std::string name)
{
	std::vector<IRCChannel>::iterator it = _channels.begin();
	std::vector<IRCChannel>::iterator ite = _channels.end();
	while (it != ite)
	{
		if (it->getName() == name)
			return &(*it);
		it++;
	}
	return NULL;
}
void Server::addChannel(IRCChannel &channel)
{
	_channels.push_back(channel);
}
void Server::deleteChannel(std::string name)
{
	std::vector<IRCChannel>::iterator it = _channels.begin();
	std::vector<IRCChannel>::iterator ite = _channels.end();
	while (it != ite)
	{
		if (it->getName() == name)
			_channels.erase(it);
		it++;
	}
}

IRCUser *Server::getOneClient(std::string nick)
{
	std::vector<IRCUser>::iterator it = _clients.begin();
	std::vector<IRCUser>::iterator ite = _clients.end();
	while (it != ite)
	{
		if (it->getNick() == nick)
			return &(*it);
		it++;
	}
	return NULL;
}
IRCUser *Server::getOneClient(int fd)
{
	std::vector<IRCUser>::iterator it = _clients.begin();
	std::vector<IRCUser>::iterator ite = _clients.end();
	while (it != ite)
	{
		if (it->getFd() == fd)
			return &(*it);
		it++;
	}
	return NULL;
}

const std::vector<IRCUser> &Server::getClients() const
{
	return _clients;
}

void Server::addClient(IRCUser &client)
{
	_clients.push_back(client);
}
void Server::deleteClient(int fd)
{
	std::vector<IRCUser>::iterator it = _clients.begin();
	std::vector<IRCUser>::iterator ite = _clients.end();
	while (it != ite)
	{
		if (it->getFd() == fd)
			_clients.erase(it);
		it++;
	}
}
Server::~Server()
{
}


