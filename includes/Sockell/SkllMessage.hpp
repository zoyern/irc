/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SkllMessage.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/16 22:03:59 by marvin            #+#    #+#             */
/*   Updated: 2025/11/16 22:03:59 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#pragma once

#include <Sockell/SkllTypes.hpp>
#include <vector>
#include <map>
#include <string>

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   MESSAGE - IRC-style message parser                                        */
/*   • Lazy parsing - bytes stay as bytes until needed                         */
/*   • Command + arguments + trailing                                          */
/*   • Parameter storage for routing variables                                 */
/* ═══════════════════════════════════════════════════════════════════════════ */

class SkllMessage {
public:
	SkllMessage();
	~SkllMessage();

	/* Set raw message (triggers parse) */
	void					set(const SkllSpan &raw);
	void					set(const byte_t *data, size_t len);

	/* Parsed components */
	SkllSpan				prefix() const;
	SkllSpan				cmd() const;
	size_t					argc() const;
	SkllSpan				arg(size_t idx) const;
	SkllSpan				trailing() const;
	SkllSpan				raw() const;

	/* Parameters (set by router for variables) */
	void					set_param(const std::string &key, const SkllSpan &val);
	void					set_param(const std::string &key, const std::string &val);
	SkllSpan				param(const std::string &key) const;
	bool					has_param(const std::string &key) const;

	/* State */
	bool					empty() const;
	void					reset();

private:
	std::vector<byte_t>					_raw;
	SkllSpan							_prefix;
	SkllSpan							_cmd;
	std::vector<SkllSpan>				_args;
	SkllSpan							_trailing;
	std::map<std::string, SkllSpan>		_params;
	std::map<std::string, std::string>	_str_params;
	bool								_parsed;

	void					_parse();

	SkllMessage(const SkllMessage &);
	SkllMessage &operator=(const SkllMessage &);
};
