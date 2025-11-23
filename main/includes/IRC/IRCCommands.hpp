/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IRCCommands.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/23 19:17:14 by marvin            #+#    #+#             */
/*   Updated: 2025/11/23 19:17:14 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <Sockell/SkllEvent.hpp>

// Authentication commands
void irc_cmd_pass(SkllEvent *event, void *user_data);
void irc_cmd_nick(SkllEvent *event, void *user_data);
void irc_cmd_user(SkllEvent *event, void *user_data);

// Channel commands
void irc_cmd_join(SkllEvent *event, void *user_data);
void irc_cmd_part(SkllEvent *event, void *user_data);
void irc_cmd_topic(SkllEvent *event, void *user_data);

// Message commands
void irc_cmd_privmsg(SkllEvent *event, void *user_data);

// Mode commands
void irc_cmd_mode(SkllEvent *event, void *user_data);
void irc_cmd_mode_i(SkllEvent *event, void *user_data);
void irc_cmd_mode_t(SkllEvent *event, void *user_data);
void irc_cmd_mode_k(SkllEvent *event, void *user_data);
void irc_cmd_mode_o(SkllEvent *event, void *user_data);
void irc_cmd_mode_l(SkllEvent *event, void *user_data);

// Operator commands
void irc_cmd_kick(SkllEvent *event, void *user_data);
void irc_cmd_invite(SkllEvent *event, void *user_data);

// Misc commands
void irc_cmd_ping(SkllEvent *event, void *user_data);
void irc_cmd_quit(SkllEvent *event, void *user_data);