/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SkllTypes.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/26 22:16:52 by marvin            #+#    #+#             */
/*   Updated: 2025/11/26 22:16:52 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#pragma once

#include <cstddef>
#include <cstring>
#include <string>
#include <sys/types.h>

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   FUNDAMENTAL TYPES                                                         */
/* ═══════════════════════════════════════════════════════════════════════════ */

typedef unsigned char	byte_t;
typedef int				fd_t;
typedef size_t			idx_t;

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   RETURN CODES                                                              */
/* ═══════════════════════════════════════════════════════════════════════════ */
#define SKLL_FATAL		-2
#define SKLL_ERR		-1
#define SKLL_OK			0
#define SKLL_STOP		1
#define SKLL_SKIP		2


/* ═══════════════════════════════════════════════════════════════════════════ */
/*   EVENT TYPES                                                               */
/* ═══════════════════════════════════════════════════════════════════════════ */

#define SKLL_EV_NONE		0
#define SKLL_EV_START		(1 << 0)
#define SKLL_EV_STOP		(1 << 1)
#define SKLL_EV_SIGNAL		(1 << 2)
#define SKLL_EV_ACCEPT		(1 << 3)
#define SKLL_EV_CLOSE		(1 << 4)
#define SKLL_EV_READ		(1 << 5)
#define SKLL_EV_WRITE		(1 << 6)
#define SKLL_EV_MESSAGE		(1 << 7)
#define SKLL_EV_ERROR		(1 << 8)

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   PROTOCOL FLAGS                                                            */
/* ═══════════════════════════════════════════════════════════════════════════ */

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   PROTOCOL FLAGS                                                            */
/*   TCP + IPv4: Fully implemented                                             */
/*   TCP + IPv6: Fully implemented                                             */
/*   TCP + IPv4 + IPv6: Dual-stack mode                                        */
/*   UDP + IPv4: Fully implemented with recvfrom/sendto                        */
/*   UDP + IPv6: Fully implemented with recvfrom/sendto                        */
/* ═══════════════════════════════════════════════════════════════════════════ */

#define SKLL_TCP		(1 << 0)  /* Fully implemented */
#define SKLL_UDP		(1 << 1)  /* Fully implemented with recvfrom/sendto */
#define SKLL_IPV4		(1 << 2)  /* Fully implemented */
#define SKLL_IPV6		(1 << 3)  /* Fully implemented */

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   TERMINAL COLORS                                                           */
/* ═══════════════════════════════════════════════════════════════════════════ */

#define SKLL_RST		"\033[0m"
#define SKLL_BOLD		"\033[1m"
#define SKLL_DIM		"\033[2m"
#define SKLL_RED		"\033[91m"
#define SKLL_GREEN		"\033[92m"
#define SKLL_YELLOW		"\033[93m"
#define SKLL_BLUE		"\033[94m"
#define SKLL_MAGENTA	"\033[95m"
#define SKLL_CYAN		"\033[96m"
#define SKLL_WHITE		"\033[97m"

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   SKLLSPAN - Zero-copy byte view                                            */
/*   Non-owning view over a byte sequence, like std::string_view (C++17)       */
/* ═══════════════════════════════════════════════════════════════════════════ */

struct SkllSpan {
	const byte_t *	data;
	size_t			len;

	/* Construction */
	SkllSpan();
	SkllSpan(const byte_t *d, size_t l);
	SkllSpan(const char *s);
	SkllSpan(const std::string &s);

	/* State */
	bool			empty() const;
	size_t			size() const;

	/* Comparison */
	bool			eq(const SkllSpan &other) const;
	bool			eq(const char *s) const;
	bool			eq(const char *s, size_t l) const;
	bool			starts(const char *s) const;
	bool			starts(const SkllSpan &other) const;

	/* Conversion */
	std::string		str() const;
	int				to_int() const;
	long			to_long() const;

	/* Subspan */
	SkllSpan		sub(size_t start, size_t length) const;
	SkllSpan		trim() const;

	/* Byte access */
	byte_t			operator[](size_t i) const;
	const byte_t *	begin() const;
	const byte_t *	end() const;
};
