/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Lobby.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/03 22:37:00 by marvin            #+#    #+#             */
/*   Updated: 2025/11/03 22:37:00 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <Sockell.hpp>

#ifndef SKLLCHANNEL
	#define CHANNEL_NAME		"general"
	#define CHANNEL_OPERATOR	-1
	#define CHANNEL_SIZE		-1
	#define CHANNEL_DEFAULT		true
	#define CHANNEL_INV_ONLY	false
	#define CHANNEL_REST_TOPIC	true
	#define CHANNEL_TOPIC		"Welcome to the general channel!"			
	#define CHANNEL_CREATED		"Created channel: "
	#define CHANNEL_EXISTS		"Existing channel used:"			
	#define CHANNEL_SET_DEFAULT	"Set to default channel :"			
#endif

class SkllChannel {
	private:
		std::string	_name;
		std::string	_password;
		std::string	_topic;
		int			_operator;
		int			_size;
		bool		_invite_only;
		bool		_restricted_topic;
public:
	SkllChannel(const std::string &name);
	~SkllChannel();

	SkllChannel	&password(const std::string& pass);
	SkllChannel	&topic(const std::string& topic);
	SkllChannel	&op(int op);
	SkllChannel	&size(int size);
	SkllChannel	&invite_only(bool inv_only);
	SkllChannel	&restricted_topic(bool rest_topic);


};