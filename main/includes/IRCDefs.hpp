/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCDefs.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/28 19:11:57 by marvin            #+#    #+#             */
/*   Updated: 2025/11/28 19:11:57 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
/* ═══════════════════════════════════════════════════════════════════════════ */
/*   IRC NUMERIC REPLIES - RFC 2812                                            */
/* ═══════════════════════════════════════════════════════════════════════════ */

/* Connection Registration */
# define RPL_WELCOME           "001"
# define RPL_YOURHOST          "002"
# define RPL_CREATED           "003"
# define RPL_MYINFO            "004"

/* Command Replies */
# define RPL_UMODEIS           "221"
# define RPL_CHANNELMODEIS     "324"
# define RPL_NOTOPIC           "331"
# define RPL_TOPIC             "332"
# define RPL_INVITING          "341"
# define RPL_NAMREPLY          "353"
# define RPL_ENDOFNAMES        "366"
# define RPL_WHOREPLY          "352"
# define RPL_ENDOFWHO          "315"

/* Error Replies */
# define ERR_NOSUCHNICK        "401"
# define ERR_NOSUCHCHANNEL     "403"
# define ERR_CANNOTSENDTOCHAN  "404"
# define ERR_NORECIPIENT       "411"
# define ERR_NOTEXTTOSEND      "412"
# define ERR_UNKNOWNCOMMAND    "421"
# define ERR_NONICKNAMEGIVEN   "431"
# define ERR_ERRONEUSNICKNAME  "432"
# define ERR_NICKNAMEINUSE     "433"
# define ERR_USERNOTINCHANNEL  "441"
# define ERR_NOTONCHANNEL      "442"
# define ERR_USERONCHANNEL     "443"
# define ERR_NOTREGISTERED     "451"
# define ERR_NEEDMOREPARAMS    "461"
# define ERR_ALREADYREGISTRED  "462"
# define ERR_PASSWDMISMATCH    "464"
# define ERR_CHANNELISFULL     "471"
# define ERR_UNKNOWNMODE       "472"
# define ERR_INVITEONLYCHAN    "473"
# define ERR_BADCHANNELKEY     "475"
# define ERR_CHANOPRIVSNEEDED  "482"

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   CHANNEL PERMISSIONS                                                       */
/* ═══════════════════════════════════════════════════════════════════════════ */

# define IRC_PERM_OP           0x01
# define IRC_PERM_VOICE        0x02
# define IRC_PERM_INVITED      0x08