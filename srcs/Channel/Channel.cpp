#include <irc.hpp>

Channel::~Channel() {}
Channel::Channel(const std::string &name)
	: _name(name)
	, _password("")
	, _topic("topic for :" + name)
	, _operator(-1), _size(-1)
	, _invite_only(false)
	, _restricted_topic(false)
{}

Channel	&Channel::password(const std::string& pass) { _password = pass; return (*this); }
Channel	&Channel::topic(const std::string& topic) { _topic = topic; return (*this); }
Channel	&Channel::op(int op) { _operator = op; return (*this); }
Channel	&Channel::size(int size) { _size = size; return (*this); }
Channel	&Channel::invite_only(bool inv_only) { _invite_only = inv_only; return (*this); }
Channel	&Channel::restricted_topic(bool rest_topic) { _restricted_topic = rest_topic; return (*this); }

