/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Sockell.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/21 22:11:32 by marvin            #+#    #+#             */
/*   Updated: 2025/11/21 22:11:32 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Sockell.hpp>

std::string Sockell::timestamp()
{
	std::time_t			now = std::time(NULL);
	std::tm				*tm_info = std::localtime(&now);
	std::ostringstream	oss;
	const char*			fmt = SKLL_TIME_HHMMSS;

	for (size_t i = 0; fmt[i]; ++i) {
		if (fmt[i] == '%' && fmt[i + 1]) {
			switch (fmt[i + 1]) {
				case 'H': if (tm_info->tm_hour < 10) oss << '0'; oss << tm_info->tm_hour; break;
				case 'M': if (tm_info->tm_min < 10) oss << '0'; oss << tm_info->tm_min; break;
				case 'S': if (tm_info->tm_sec < 10) oss << '0'; oss << tm_info->tm_sec; break;
				default: oss << fmt[i] << fmt[i + 1]; break;
			}
			++i;
		}
		else oss << fmt[i];
	}
	return (oss.str());
}
