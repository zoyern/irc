/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SkllLog.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/27 19:54:01 by marvin            #+#    #+#             */
/*   Updated: 2025/11/27 19:54:01 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#pragma once

#include <Sockell/SkllTypes.hpp>
#include <iostream>
#include <sstream>
#include <ctime>

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   LOG - Static logging interface                                            */
/* ═══════════════════════════════════════════════════════════════════════════ */

class SkllLog {
public:
	template <typename T>
	static void	info(const T &msg);

	template <typename T>
	static void	warn(const T &msg);

	template <typename T>
	static void	error(const T &msg);

	template <typename T>
	static void	debug(const T &msg);

	template <typename T>
	static void	success(const T &msg);

	static std::string		timestamp();
	static void				set_debug(bool enabled);
	static bool				debug_enabled();

private:
	static bool				_debug;
	SkllLog();
};

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   TEMPLATE IMPLEMENTATIONS                                                  */
/* ═══════════════════════════════════════════════════════════════════════════ */

template <typename T>
void SkllLog::info(const T &msg) {
	std::cout << SKLL_CYAN << "[" << timestamp() << "] INFO " << SKLL_RST << msg << std::endl;
}

template <typename T>
void SkllLog::warn(const T &msg) {
	std::cout << SKLL_YELLOW << "[" << timestamp() << "] WARN " << SKLL_RST << msg << std::endl;
}

template <typename T>
void SkllLog::error(const T &msg) {
	std::cerr << SKLL_RED << "[" << timestamp() << "] ERROR " << SKLL_RST << msg << std::endl;
}

template <typename T>
void SkllLog::debug(const T &msg) {
	if (_debug)
		std::cout << SKLL_DIM << "[" << timestamp() << "] DEBUG " << SKLL_RST << msg << std::endl;
}

template <typename T>
void SkllLog::success(const T &msg) {
	std::cout << SKLL_GREEN << "[" << timestamp() << "] OK " << SKLL_RST << msg << std::endl;
}
