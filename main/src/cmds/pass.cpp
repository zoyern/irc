#include "Irc.hpp"

void pass_cmd(SkllEvent *event, void *user_data)
{
	Server *server = static_cast<Server *>(user_data);
	Client *client = (Client *)event->client->userdata;

	if (!client)
		return;
	if (client->isAuth())
		return client->send(irc_err_alreadyregistred(client->getNick()));;
	std::string password = event->message->to<std::string>();
	if(password.empty())
		return client->send(irc_err_needmoreparams(client->getNick(), password)); //ERR_NEEDMOREPARAMS
	if (server->checkPassword(password))
		client->setIsPassCorrect(true);
}
