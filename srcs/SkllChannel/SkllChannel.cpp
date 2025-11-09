/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SkllChannel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/07 20:47:38 by marvin            #+#    #+#             */
/*   Updated: 2025/11/07 20:47:38 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Sockell/SkllChannel.hpp>

SkllChannel::~SkllChannel() {}
SkllChannel::SkllChannel(const std::string &name)
	: _name(name)
	, _password("")
	, _topic("topic for :" + name)
	, _operator(-1), _size(-1)
	, _invite_only(false)
	, _restricted_topic(false)
{}

SkllChannel	&SkllChannel::password(const std::string& pass) { _password = pass; return (*this); }
SkllChannel	&SkllChannel::topic(const std::string& topic) { _topic = topic; return (*this); }
SkllChannel	&SkllChannel::op(int op) { _operator = op; return (*this); }
SkllChannel	&SkllChannel::size(int size) { _size = size; return (*this); }
SkllChannel	&SkllChannel::invite_only(bool inv_only) { _invite_only = inv_only; return (*this); }
SkllChannel	&SkllChannel::restricted_topic(bool rest_topic) { _restricted_topic = rest_topic; return (*this); }

