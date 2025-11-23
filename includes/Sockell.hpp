/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Sockell.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 22:35:03 by marvin            #+#    #+#             */
/*   Updated: 2025/11/03 22:35:03 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <ctime>
#include <sstream>
#include <string>
#include <map>

#define SKLL_RESET      "\033[0m"
#define SKLL_BOLD       "\033[1m"
#define SKLL_DIM        "\033[2m"
#define SKLL_RED        "\033[31m"
#define SKLL_GREEN      "\033[32m"
#define SKLL_YELLOW     "\033[33m"
#define SKLL_BLUE       "\033[34m"
#define SKLL_MAGENTA    "\033[35m"
#define SKLL_CYAN       "\033[36m"
#define SKLL_WHITE      "\033[37m"
#define SKLL_GRAY       "\033[90m"

#define SKLL_ON_START       (1 << 0)
#define SKLL_ON_UPDATE      (1 << 1)
#define SKLL_ON_STOP        (1 << 2)
#define SKLL_ON_SHUTDOWN    (1 << 3)
#define SKLL_ON_CONNECT     (1 << 4)
#define SKLL_ON_DISCONNECT  (1 << 5)
#define SKLL_ON_ERROR       (1 << 6)
#define SKLL_ON_TIMEOUT     (1 << 7)
#define SKLL_ON_RECV        (1 << 8)
#define SKLL_ON_SEND        (1 << 9)
#define SKLL_ON_SIGNAL      (1 << 10)

#define SKLL_TCP    (1 << 0)
#define SKLL_UDP    (1 << 1)
#define SKLL_IPV4   (1 << 2)
#define SKLL_IPV6   (1 << 3)

#define SKLL_TIME_HHMMSS "%H:%M:%S"

struct Sockell { static std::string timestamp();};
