/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SkllLog.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/27 20:00:43 by marvin            #+#    #+#             */
/*   Updated: 2025/11/27 20:00:43 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include <Sockell/SkllLog.hpp>
#include <iomanip>

bool SkllLog::_debug = false;

std::string SkllLog::timestamp() {
	std::time_t now = std::time(0);
	std::tm *t = std::localtime(&now);
	std::ostringstream oss;
	oss << std::setfill('0')
		<< std::setw(2) << t->tm_hour << ":"
		<< std::setw(2) << t->tm_min << ":"
		<< std::setw(2) << t->tm_sec;
	return oss.str();
}

void SkllLog::set_debug(bool enabled) { _debug = enabled; }
bool SkllLog::debug_enabled() { return _debug; }
