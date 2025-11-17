/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Lobby.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 22:37:00 by marvin            #+#    #+#             */
/*   Updated: 2025/11/03 22:37:00 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <Sockell/SkllHook.hpp>
#include <string>
#include <set>

class SkllChannel {
	public:
		SkllHook				hook;
		std::string				name;
		std::set<std::string>	client_ids;
		void					*userdata;

		~SkllChannel();
		SkllChannel();
		SkllChannel(const std::string &n);

		SkllChannel	&on(int event, SkllHook::Callback cb, void *user_data = NULL);
		void		add_client(const std::string &id);
		void		remove_client(const std::string &id);

		bool		has_client(const std::string &id) const;
		size_t		size() const;
};
