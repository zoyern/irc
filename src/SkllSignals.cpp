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

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   STATIC STORAGE FOR SIGNAL HANDLER                                          */
/*   Required by POSIX - signal handlers cannot access instance data            */
/* ═══════════════════════════════════════════════════════════════════════════ */

volatile sig_atomic_t SkllSignals::_ring[SKLL_SIG_QUEUE_SIZE];
volatile sig_atomic_t SkllSignals::_head = 0;
volatile sig_atomic_t SkllSignals::_tail = 0;

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   LIFECYCLE                                                                  */
/* ═══════════════════════════════════════════════════════════════════════════ */

SkllSignals::SkllSignals() : _setup(false) {}
SkllSignals::~SkllSignals() { restore(); }

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   CONFIGURATION                                                              */
/* ═══════════════════════════════════════════════════════════════════════════ */

SkllSignals &SkllSignals::trap(int sig) {
	_trapped.push_back(sig);
	return *this;
}

SkllSignals &SkllSignals::trap_int()  { return trap(SIGINT); }
SkllSignals &SkllSignals::trap_term() { return trap(SIGTERM); }
SkllSignals &SkllSignals::trap_pipe() { return trap(SIGPIPE); }
SkllSignals &SkllSignals::trap_hup()  { return trap(SIGHUP); }

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   SIGNAL HANDLER - Called by OS, must be signal-safe                         */
/*   Only uses sig_atomic_t operations - no malloc, no locks, no stdio          */
/* ═══════════════════════════════════════════════════════════════════════════ */

void SkllSignals::handler(int sig) {
	/* Calculate next position with wrap-around */
	sig_atomic_t next = (_head + 1) % SKLL_SIG_QUEUE_SIZE;
	
	/* Drop signal if buffer full (cannot block or allocate in signal handler) */
	if (next == _tail) return;
	
	_ring[_head] = sig;
	_head = next;
}

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   SETUP / RESTORE                                                            */
/* ═══════════════════════════════════════════════════════════════════════════ */

bool SkllSignals::setup() {
	struct sigaction sa;
	sa.sa_handler = handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	
	for (size_t i = 0; i < _trapped.size(); ++i) {
		if (sigaction(_trapped[i], &sa, 0) < 0)
			return false;
	}
	
	_setup = true;
	return true;
}

void SkllSignals::restore() {
	if (!_setup) return;
	
	struct sigaction sa;
	sa.sa_handler = SIG_DFL;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	
	for (size_t i = 0; i < _trapped.size(); ++i)
		sigaction(_trapped[i], &sa, 0);
	
	_setup = false;
}

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   QUEUE ACCESS - Main thread only                                            */
/* ═══════════════════════════════════════════════════════════════════════════ */

bool SkllSignals::pending() const {
	return _head != _tail;
}

int SkllSignals::pop() {
	if (_head == _tail) return 0;
	
	int sig = static_cast<int>(_ring[_tail]);
	_tail = (_tail + 1) % SKLL_SIG_QUEUE_SIZE;
	return sig;
}
