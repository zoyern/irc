/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 22:35:03 by marvin            #+#    #+#             */
/*   Updated: 2025/11/03 22:35:03 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <cstdlib>
#include <string>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <map>
#include <iostream>
#include <vector>

#include <irc/Server/Epoll.hpp>
#include <irc/Server.hpp>
#include <irc/Client.hpp>
#include <irc/Channel.hpp>
#include <irc/Console.hpp>
