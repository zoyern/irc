#include <irc.hpp>

Channel::Channel(void) :_name("NULL")
{}
Channel::Channel(std::string name): _name(name)
{return;}
Channel::Channel(const Channel& other);
Channel& Channel::operator=(const Channel& other);
Channel& Channel::name(const std::string& name)
{
	_name = name;
}
Channel& Channel::password(const std::string& pass)
{
	_password = pass;
}
// Channel& Channel::op(const std::string& name)
// {
// 	_op =
// }
// Channel& Channel::size(const std::string& address);
// Channel& Channel::invite_only(const std::string &msg);
// Channel& Channel::restricted_topic (int max);
// Channel& Channel::topic(int reserved);
Channel::~Channel(void){return;}
