/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Hook.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/09 00:50:10 by marvin            #+#    #+#             */
/*   Updated: 2025/11/09 00:50:10 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Sockell/SkllHook.hpp>

SkllHook::SkllHook() {}
SkllHook::~SkllHook() {}

SkllHook &SkllHook::on(int event_mask, skllCallback callback, void *data)
{
	t_skllCallData cb;
	cb.call = callback;
	cb.data = data;

	// Parcours des bits actifs du mask
	for (int bit = 0; bit < 32; ++bit)
	{
		int flag = 1 << bit;
		if (event_mask & flag)
			_callbacks[(e_skllevent)flag].push_back(cb);
	}
	return (*this);
}

SkllHook &SkllHook::on(const std::string &event_mask, skllCallback callback, void *data) { (void)event_mask;(void)callback;(void)data; return( *this);}

void SkllHook::trigger(e_skllevent event)
{
	std::map<e_skllevent, std::vector<t_skllCallData> >::iterator it = _callbacks.find(event);
	if (it == _callbacks.end())
		return;

	for (std::vector<t_skllCallData>::iterator cb = it->second.begin(); cb != it->second.end(); ++cb)
		cb->call();
}