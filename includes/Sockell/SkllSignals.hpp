/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SkllSignals.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/17 02:31:16 by marvin            #+#    #+#             */
/*   Updated: 2025/11/17 02:31:16 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#pragma once

#include <Sockell/SkllTypes.hpp>
#include <deque>
#include <csignal>

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   SIGNAL QUEUE SIZE - Maximum pending signals before overflow               */
/*   NOTE: This is a necessary limit for signal-safe operation                 */
/*         Signals are OS-level events with inherent limits                    */
/* ═══════════════════════════════════════════════════════════════════════════ */

#define SKLL_SIG_QUEUE_SIZE 64

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   SIGNALS - Signal-safe handling with ring buffer                           */
/*                                                                             */
/*   IMPLEMENTATION NOTES:                                                     */
/*   • Static storage is REQUIRED by POSIX signal handling (handler cannot    */
/*     access instance data or receive user parameters)                        */
/*   • Uses volatile sig_atomic_t for signal-safe operation                    */
/*   • Ring buffer avoids memory allocation in signal handler                  */
/*   • Single producer (signal handler), single consumer (main thread)         */
/* ═══════════════════════════════════════════════════════════════════════════ */

class SkllSignals {
public:
	SkllSignals();
	~SkllSignals();

	/* Configuration (fluent) */
	SkllSignals &		trap(int sig);
	SkllSignals &		trap_int();
	SkllSignals &		trap_term();
	SkllSignals &		trap_pipe();
	SkllSignals &		trap_hup();

	/* Setup */
	bool				setup();
	void				restore();

	/* Queue access (main thread only) */
	bool				pending() const;
	int					pop();

	/* Static handler - called by OS */
	static void			handler(int sig);

private:
	std::deque<int>		_trapped;
	bool				_setup;

	/*
	 *SIGNAL-SAFE RING BUFFER
	 *Static because signal handlers cannot access instance data (POSIX constraint)
	 *volatile sig_atomic_t ensures signal-safe read/write
	 */
	static volatile sig_atomic_t	_ring[SKLL_SIG_QUEUE_SIZE];
	static volatile sig_atomic_t	_head;		/* Write position (signal handler) */
	static volatile sig_atomic_t	_tail;		/* Read position (main thread)     */

	SkllSignals(const SkllSignals &);
	SkllSignals &operator=(const SkllSignals &);
};
