#include "Irc.hpp"

void quit_cmd(SkllEvent *event, void *user_data)
{
	Server *server = static_cast<Server *>(user_data);
	Client *client = (Client *)event->client->userdata;

	server->deleteClient(client->getFd());
}
