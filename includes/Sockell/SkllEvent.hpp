/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SkllEvent.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/13 17:49:20 by marvin            #+#    #+#             */
/*   Updated: 2025/11/13 17:49:20 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <string>

#define SKLL_HOOK_SIZE 9

enum e_skllhook {
	ON_START		= 1 << 0,
	ON_UPDATE		= 1 << 1,
	ON_SHUTDOWN		= 1 << 2,
	ON_CONNECT		= 1 << 3,
	ON_DISCONNECT	= 1 << 4,
	ON_ERROR		= 1 << 5,
	ON_TIMEOUT		= 1 << 6,
	ON_RECV			= 1 << 7,
	ON_SEND			= 1 << 8,
};

class SkllEvent {
public:
	e_skllhook	type;
	void		*data;
	std::string	source;

	SkllEvent(e_skllhook t = ON_START, void *d = NULL)
		: type(t), data(d), source("") {}
};
