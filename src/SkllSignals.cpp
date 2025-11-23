/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SkllSignals.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/17 02:31:48 by marvin            #+#    #+#             */
/*   Updated: 2025/11/17 02:31:48 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Sockell/SkllSignals.hpp>

int         SkllSignals::_signal_fd = -1;
sigset_t    SkllSignals::_mask;


int SkllSignals::setup() {
	sigemptyset(&_mask);
	sigaddset(&_mask, SIGINT);
	sigaddset(&_mask, SIGTERM);
	sigaddset(&_mask, SIGQUIT);

	signal(SIGPIPE, SIG_IGN);

	if (sigprocmask(SIG_BLOCK, &_mask, NULL) < 0)
		return -1;

	_signal_fd = signalfd(-1, &_mask, SFD_NONBLOCK | SFD_CLOEXEC);
	if (_signal_fd < 0)
		return -1;
	
	std::cout << SKLL_GRAY << "[Signals] Setup complete (fd=" 
			  << _signal_fd << ")" << SKLL_RESET << std::endl;
	return _signal_fd;
}

void SkllSignals::cleanup() {
	if (_signal_fd < 0) return;
	close(_signal_fd);
	_signal_fd = -1;
	sigprocmask(SIG_UNBLOCK, &_mask, NULL);
}

int SkllSignals::get_fd() {
	return _signal_fd;
}

int SkllSignals::read_signal() {
	signalfd_siginfo info;
	if (_signal_fd < 0) return -1;
	if (read(_signal_fd, &info, sizeof(info)) != sizeof(info)) return -1;
	return info.ssi_signo;
}

SkllSignals::~SkllSignals() { cleanup(); }
SkllSignals::SkllSignals() {}
SkllSignals::SkllSignals(const SkllSignals&) {}
SkllSignals &SkllSignals::operator=(const SkllSignals&) { return *this; }