/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCUser.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/28 19:12:18 by marvin            #+#    #+#             */
/*   Updated: 2025/11/28 19:12:18 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include <Sockell/SkllTypes.hpp>
# include <string>

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   IRC USER - Simple POD structure for user state                            */
/* ═══════════════════════════════════════════════════════════════════════════ */

struct IRCUser {
	fd_t        fd;
	std::string nick;
	std::string user;
	std::string realname;
	std::string host;
	bool        pass_ok;
	bool        registered;

	IRCUser() : fd(-1), pass_ok(false), registered(false) {}
};