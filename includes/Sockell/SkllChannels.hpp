/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SkllChannels.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/28 21:46:31 by marvin            #+#    #+#             */
/*   Updated: 2025/11/28 21:46:31 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <Sockell/SkllChannel.hpp>
/* ═══════════════════════════════════════════════════════════════════════════ */
/*   CHANNELS - Collection manager with O(log n) lookup                        */
/*                                                                             */
/*   OPTIMIZATION: Reverse index fd → channels for O(k) removal                */
/*   where k = number of channels a user is in (typically small)               */
/* ═══════════════════════════════════════════════════════════════════════════ */

class SkllChannels
{
	public:
		typedef std::map<std::string, SkllChannel>::iterator		iterator;
		typedef std::map<std::string, SkllChannel>::const_iterator	const_iterator;
	private:
		std::map<std::string, SkllChannel>		_channels;
		std::map<fd_t, std::set<std::string> >	_fd_channels;
	public:
		~SkllChannels();
		SkllChannels();
		SkllChannels(const SkllChannels &other);
		SkllChannels	&operator=(const SkllChannels &other);

		void								clear();
		void								remove(const std::string &name);
		void								remove_all(fd_t fd);
		void								join(fd_t fd, const std::string &chan_name, size_t perms = 0);
		void								part(fd_t fd, const std::string &chan_name);
		iterator							begin();
		iterator							end();
		SkllChannel							*create(const std::string &name);
		SkllChannel							*get(const std::string &name);
		std::map<std::string, SkllChannel>	&all();

		bool										exists(const std::string &name) const;
		void										of(fd_t fd, std::set<std::string> &result) const;
		size_t										count() const;
		const_iterator								begin() const;
		const_iterator								end() const;
		const SkllChannel							*get(const std::string &name) const;
		const std::set<std::string> 				*channels_of(fd_t fd) const;
		const std::map<std::string, SkllChannel>	&all() const;
	private:
		void						_index_add(fd_t fd, const std::string &chan);
		void						_index_remove(fd_t fd, const std::string &chan);
		std::string					_normalize(const std::string &s) const;
};
