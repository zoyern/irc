#include "Main.hpp"

void invite(int fd, std::string nickName)
{
	// cmd: INVITE nickName Channel
	// verify if user is a memeber of channel
	// verify if target exist
	// verify if target user is not already on channel, or
	// hes been already invited

	// sent message to invited user only
}

// epoll -> event ->recv
// recv -> message ->

// message parser
// "NICK user"
// "#general user"

// main channel(genral)
// 	.on(ON_CONNECT, registerclientchannel, irc)

// 	registerclientchannel(irc, eventserver)
// 	Client
// 	serveur
// 	Protocol
// 	message
// 	channel
// if (#)
// 	get_channel();

// channel.add(user);

//  s stringstream(general)
//  s << int
//  Message.() <<
//  Message.toint tostirng coucu salut <<
// -> INVITE
// INVITE -> irc.getclient("general")
