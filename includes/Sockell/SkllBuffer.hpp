/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SkllBuffer.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/26 22:18:04 by marvin            #+#    #+#             */
/*   Updated: 2025/11/26 22:18:04 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <Sockell/SkllTypes.hpp>
#include <deque>

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   BUFFER - Dynamic circular buffer using std::deque                         */
/*   • No fixed size limit                                                     */
/*   • Efficient push_back/pop_front via deque                                 */
/*   • Zero-copy line extraction                                               */
/*   • Copyable (for storage in containers)                                    */
/* ═══════════════════════════════════════════════════════════════════════════ */

class SkllBuffer
{
	private:
		std::string			_line_buf;
		std::deque<byte_t>	_data;
	public:
		~SkllBuffer();
		SkllBuffer();
		SkllBuffer(const SkllBuffer &other);
		SkllBuffer	&operator=(const SkllBuffer &other);

		void			reset();
		void			skip(size_t len);
		size_t			write(const byte_t *data, size_t len);
		size_t			write(const char *str);
		size_t			write(const std::string &str);
		size_t			write(const SkllSpan &span);
		size_t			read(byte_t *dst, size_t len);
		size_t			peek(byte_t *dst, size_t len) const;
		SkllSpan		line(const byte_t *delim, size_t dlen);
		SkllSpan		line(const char *delim);

		bool			empty() const;
		size_t			size() const;
		ssize_t			find(byte_t c) const;
		ssize_t			find(const byte_t *seq, size_t len) const;
		byte_t			operator[](size_t i) const;
};
