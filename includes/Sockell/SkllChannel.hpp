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
#include <Sockell/SkllTypes.hpp>
#include <string>
#include <set>
#include <map>
/* ═══════════════════════════════════════════════════════════════════════════ */
/*   CHANNEL - Generic group/room with custom permissions                      */
/*   • Permissions defined by CLIENT                                           */
/*   • Groups of permissions (operator, viewer, etc.) defined by client        */
/*   • Members tracked by fd                                                   */
/*   • Copyable (stored in std::map by value, no heap allocation)              */
/* ═══════════════════════════════════════════════════════════════════════════ */

class SkllChannel
{
	private:
		std::string							_name;
		std::set<fd_t>						_members;
		std::map<fd_t, size_t>				_perms;
		std::map<std::string, std::string>	_meta;

		static std::string					_empty;
	public:
		~SkllChannel();
		SkllChannel();
		SkllChannel(const std::string &name);
		SkllChannel(const SkllChannel &other);
		SkllChannel	&operator=(const SkllChannel &other);

		void						clear();
		bool						add(fd_t fd, size_t perms = 0);
		void						set_name(const std::string &name);
		void						remove(fd_t fd);

		void						set_perms(fd_t fd, size_t p);
		void						add_perms(fd_t fd, size_t p);
		void						del_perms(fd_t fd, size_t p);

		void						del(const std::string &key);
		void						set(const std::string &key, const std::string &val);

		bool						empty() const;
		bool						has(fd_t fd) const;
		bool						has(const std::string &key) const;
		bool						has_perm(fd_t fd, size_t p) const;
		size_t						count() const;
		size_t						perms(fd_t fd) const;
		const std::set<fd_t>		&members() const;
		const std::string			&name() const;
		const std::string			&get(const std::string &key) const;
};

