/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SkllTCP.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/11 21:01:36 by marvin            #+#    #+#             */
/*   Updated: 2025/11/11 21:01:36 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Sockell/SkllProtocol.hpp>

    // Options spécifiques à TCP (niveau IPPROTO_TCP)
    OPT_TCP_NODELAY,           // Désactive l'algorithme de Nagle (agrégation de petits paquets)
    OPT_TCP_QUICK_ACK,         // Désactive le délai d'accusé de réception
    OPT_TCP_KEEPALIVE_IDLE,    // Temps d'inactivité avant d'envoyer le premier paquet keepalive
    OPT_TCP_KEEPALIVE_INTERVAL,// Intervalle entre les paquets keepalive
    OPT_TCP_KEEPALIVE_COUNT,   // Nombre de paquets keepalive non acquittés avant de considérer la connexion comme morte
    OPT_TCP_USER_TIMEOUT,      // Temps maximum sans réponse avant de fermer la connexion
    OPT_TCP_CORK, 