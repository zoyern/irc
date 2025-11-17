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
#include <Sockell.hpp>
#include <string>
#include <cstring>
#include <arpa/inet.h>

class SkllMessage {
	private:
		SkllBuffer	&_buf;
	public:
		~SkllMessage();
		SkllMessage(SkllBuffer &b);

		bool	has_line(const char *delim, size_t dlen) const;
		size_t	extract_line(SkllBuffer &out, const char *delim, size_t dlen);

		size_t					size() const;
		const unsigned char		*data() const;
		template<typename T> T	to() const;
};

#include <Sockell/SkllMessage/SkllMessage.tpp>