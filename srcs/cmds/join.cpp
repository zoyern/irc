#include <irc.hpp>

void join(int fd, std::string password)
{
	// if channel is invite only, verify if user has been invited.
	// 	if user have been invited, then add user, and delete user
	// 	from invitedlist

	// if channel required a password, compare password
	// 	if passwords are the same, then add userw

	// brodcast to all client off the channel
}
