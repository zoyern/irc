/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SkllHook.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/09 01:23:17 by marvin            #+#    #+#             */
/*   Updated: 2025/11/09 01:23:17 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <Sockell.hpp>

enum e_skllevent {
	ON_START		= 1 << 0,
	ON_UPDATE		= 1 << 1,
	ON_SHUTDOWN		= 1 << 2,
	ON_CONNECT		= 1 << 3,
	ON_DISCONNECT	= 1 << 4,
	ON_ERROR		= 1 << 5,
	ON_TIMEOUT		= 1 << 6,
	ON_SEND			= 1 << 7,
	ON_RECV			= 1 << 8
};

class SkllHook
{
	public:
		typedef void (*skllCallback)();
		struct t_skllCallData {skllCallback call;	void *data; };
	private:
		std::map<e_skllevent, std::vector<t_skllCallData> > _callbacks;
public:
	SkllHook();
	~SkllHook();

	SkllHook	&on(int event, skllCallback callback, void *data = NULL);
	SkllHook	&on(const std::string &event, skllCallback callback, void *data = NULL);
	void		trigger(e_skllevent event);
};

