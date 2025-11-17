/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/06 03:55:49 by marvin            #+#    #+#             */
/*   Updated: 2025/11/06 03:55:49 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Sockell/SkllClient.hpp>

SkllClient::SkllClient() : fd(-1), id(""), userdata(NULL) {}
SkllClient::~SkllClient() {}

SkllClient& SkllClient::on(int event, SkllHook::Callback cb, void* user_data) {
    hook.on(event, cb, user_data);
    return *this;
}