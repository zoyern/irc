#include <irc.hpp>

void kick(int adminFd, int clientFd, std::string reason)
{
	// verify that the user who write the command is admin
	// verify that the user who is kicked is in the channel

	// broadcast to all user of the channel exept the user kicked
}
