#pragma once
#include <IRC.hpp>

class IRCChannel
{
private:
	std::vector<IRCUser> _clients;
	std::vector<IRCUser> _operators;
	std::vector<IRCUser> _inviteds;
	std::vector<std::pair<char, bool> > modes;
	std::string _name;
	std::string _password;
	std::string _topic;
	// int _operator;
	int _size;
	bool _inviteOnly;
	bool _restrictedTopic;

public:
	IRCChannel(void);
	IRCChannel(std::string name, IRCUser &client);
	IRCChannel(std::string name);
	IRCChannel &operator=(const std::string &name);
	~IRCChannel();

	// SETTER
	void setName(std::string name);
	void setPassword(std::string password);
	void setTopic(std::string topic);
	// void setOperator(int op);
	void setSize(int size);
	void setInviteOnly(bool inv_only);
	void SetRestrictedTopic(bool rest_topic);

	// GETTER
	std::string getName(void) const;
	std::string getPassword(void) const;
	std::string getTopic(void) const;
	int getSize(void) const;
	bool getInviteOnly(void) const;
	bool getRestrictedTopic(void) const;
	IRCUser *getClient(int fd);
	IRCUser *getClient(std::string nick);
	IRCUser *getOperator(int fd);
	IRCUser *getInvitedClient(int fd);

	// ADD FUNC
	void addClient(IRCUser &newClient);
	void addOperator(IRCUser &newOperator);
	void addInvitation(IRCUser &newOperator);

	// REMOVE CLIENT
	void removeClient(int fd);
	void removeOperator(int fd);
	void removeInvitedClient(int fd);
	void broadcastToOne(int fd, std::string msg);
	void broadcastToAll(std::string msg);
	void broadcastToAll(int fd, std::string msg);
};

// Un IRCChannel est un objet logique qui regroupe :
// un nom (#general, #42)
// une liste de membres
// une liste d’opérateurs
// un topic
// des modes (+i, +t, +k, +l)
// éventuellement une key (pour +k)
// éventuellement une limite (pour +l)
// une liste d’utilisateurs invités (pour +i)

// a) Le client envoie : JOIN #nom [key]
// Ton serveur doit :
// Vérifier l’existence du IRCChannel
// S’il n’existe pas → créer le IRCChannel
// Le client qui le crée devient opérateur par défaut
// Vérifier les modes du IRCChannel
// +i → client doit être invité
// +k → vérifier la key
// +l → vérifier qu’il reste une place
// +t → n’affecte pas JOIN, juste le changement de topic

// Ajouter le client à la liste des membres
// Ajouter son identifiant/FD dans la liste des membres du IRCChannel
// Ajouter le IRCChannel dans la liste de IRCChannels du client (client.joinedIRCChannels)
// Notifier tous les autres membres
// Broadcast du JOIN (sauf au client lui-même)
// Envoyer au client la liste complète des membres
// Format NAMES (353)
// Puis END OF NAMES (366)

// Un JOIN d’un invité doit bypasser +i

// COMMAND PART
// losrqu'un utilisateur par:
// 	verifier si le IRCChannel existe
// 	verifier qu'il en est membre, si oui olors retirer de la liste
// 	verifier s'il est operateur, si oui olors retirer de la liste
// 	si le IRCChannel est vide supprimer le IRCChannel
//	si le chnnel n'a plus d'operateur, promouvoir le plus ancien