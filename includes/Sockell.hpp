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
#include <vector>
#include <stdint.h>

typedef std::vector<unsigned char> SkllBuffer;

enum SkllProtocolType {
    SKLL_TCP  = 1 << 0,
    SKLL_UDP  = 1 << 1,
    SKLL_IPV4 = 1 << 2,
    SKLL_IPV6 = 1 << 3
};

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

struct SkllHookData {
    void* lib_data;
    void* user_data;
};
