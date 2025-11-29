/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/21 23:14:52 by marvin            #+#    #+#             */
/*   Updated: 2025/11/21 23:14:52 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#pragma once
#include <IRC.hpp>


class IRCServer
{
private:
    fd_t    _tcpfd;
    fd_t    _epfd;

    std::string _password;

    //TCP
    int     _port;
    std::string  _adress; // 0.0.0.0

    int    _socket_create();
    int    _epoll_create();

    int     _handle_wait();
    int     _handle_connect();
    int     _handle_message(struct epoll_event &e);
    int     _handle_error(struct epoll_event &e);
    int     _handle_disconnect(struct epoll_event &e);
    int	    _handle_root(struct epoll_event &e);

    int     _add_client();
    int     _add_chans();
    int     _remove_client();
    int     _remove_chans();

    
    
    IRCUser     &client(fd_t fd);
    IRCUser     &client(const std::string &name);
    IRCChannel  &chan(const std::string &name);


    void	_epoll_add(fd_t fd);

    std::map<fd_t, IRCUser*>             _clients;
    std::map<std::string, IRCChannel*>   _chans;

public:
    ~IRCServer();
    IRCServer(int port = 6667, const std::string &password = "pass");

    int    run();
    int    stop();
    int    clean();
};


class IRCUser;
class Server
{
public:
	Server(int port, std::string password);
	void initServer(void);
	static void setSignal(bool b);
	~Server();
	IRCChannel *getOneChannel(std::string name);
	const std::vector<IRCChannel> &getChannels() const;
	void addChannel(IRCChannel &channel);
	void deleteChannel(std::string name);
	IRCUser *getOneClient(int fd);
	IRCUser *getOneClient(std::string nick);
	const std::vector<IRCUser> &getClients() const;
	void addClient(IRCUser &client);
	void deleteClient(int fd);

	void cmd_pass(std::vector<std::string> cmd, IRCUser *client);

private:
	int _port;
    std::string _password;
	int _serverFd;
	int _epollFd;
	static bool _signal;
	struct sockaddr_in _serverAddress;
	struct sockaddr_in _clientAddress;
	struct epoll_event _event;
	struct epoll_event _events[128];
	void _setSocket(void);
	void _handleNewClient(void);
	void _handleClientData(int fd);
	void _handleCmd(std::string str, IRCUser *client);
	std::vector<IRCChannel> _channels;
	std::vector<IRCUser> _clients;
};