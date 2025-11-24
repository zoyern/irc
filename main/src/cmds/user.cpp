#include "Irc.hpp"

void user_cmd(SkllEvent *event, void *user_data)
{
	Server *server = static_cast<Server *>(user_data);
	Client *client = (Client *)event->client->userdata;
	if (!client) return;
	if(!client->getisPassCorrect())
		return client->send("Error: Server need passWord (PASS <server password>\n)");
	std::string user = event->message->to<std::string>();
	if(user.empty())
		return client->send("Commande error: need more params\n"); //ERR_NEEDMOREPARAMS
	client->setUser(user);
	if(client->isAuth())
		client->send("Welcome " + client->getNick() + "\n");
}
