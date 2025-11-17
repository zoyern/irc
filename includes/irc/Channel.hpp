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

#ifndef CHANNEL
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

#include "../irc.hpp"
class Channel {
	private:
		std::vector<Client> _clients;
		std::vector<Client> _operators;
		std::vector<Client> _inviteds;
		std::vector<std::pair<char, bool> > modes;
		std::string	_name;
		std::string	_password;
		std::string	_topic;
		int			_operator;
		int			_size;
		bool		_inviteOnly;
		bool		_restrictedTopic;
	public:
		Channel(void);
		Channel(std::string name, Client &client);
		Channel(std::string name);
		Channel &operator=(const std::string &name);
		~Channel();

		// SETTER
		void			setName(std::string name);
		void			setPassword(std::string password);
		void			setTopic(std::string topic);
		void			setOperator(int op);
		void			setSize(int size);
		void			setInviteOnly(bool inv_only);
		void			SetRestrictedTopic(bool rest_topic);

		// GETTER
		std::string 	getName(void) const;
		std::string 	getPassword(void) const;
		std::string 	getTopic(void) const;
		int				getSize(void) const;
		bool			getInviteOnly(void) const;
		bool			getRestrictedTopic(void) const;
		Client			*getClient(int fd);
		Client			*getOperator(int fd);
		Client			*getInvitedClient(int fd);

		// ADD FUNC
		void addClient(Client &newClient);
		void addOperator(Client &newOperator);
		void addInvitation(Client &newOperator);

		// REMOVE CLIENT
		void removeClient(int fd);
		void removeOperator(int fd);
		void removeInvitedClient(int fd);
		void broadcastToAll(int fd);
		void broadcastToAll(int fd, std::string msg);
};


// Un channel est un objet logique qui regroupe :
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
// Vérifier l’existence du channel
// S’il n’existe pas → créer le channel
// Le client qui le crée devient opérateur par défaut
// Vérifier les modes du channel
// +i → client doit être invité
// +k → vérifier la key
// +l → vérifier qu’il reste une place
// +t → n’affecte pas JOIN, juste le changement de topic

// Ajouter le client à la liste des membres
// Ajouter son identifiant/FD dans la liste des membres du channel
// Ajouter le channel dans la liste de channels du client (client.joinedChannels)
// Notifier tous les autres membres
// Broadcast du JOIN (sauf au client lui-même)
// Envoyer au client la liste complète des membres
// Format NAMES (353)
// Puis END OF NAMES (366)

// Un JOIN d’un invité doit bypasser +i

// COMMAND PART
// losrqu'un utilisateur par:
// 	verifier si le channel existe
// 	verifier qu'il en est membre, si oui olors retirer de la liste
// 	verifier s'il est operateur, si oui olors retirer de la liste
// 	si le channel est vide supprimer le channel
//	si le chnnel n'a plus d'operateur, promouvoir le plus ancien
