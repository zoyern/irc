/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hook_channel.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/09 01:32:21 by marvin            #+#    #+#             */
/*   Updated: 2025/11/09 01:32:21 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Sockell.hpp>

void on_channel_start(SkllChannel &srv, void* data) {
    (void)data;
    srv.console.info("Channel") << "Channel started on " << srv.address() << ":" << srv.port();
}

void on_channel_update(SkllChannel &srv, void* data) {
    (void)srv;
	(void)data;
}

void on_channel_shutdown(SkllChannel &srv, void* data) {
    (void)data;
    srv.console.info("Channel") << "Channel shutting down.";
}

void on_channel_connect(SkllChannel &srv, void* data) {
    int fd = *reinterpret_cast<int*>(data);
    srv.console.info("Channel") << "Client connected: fd=" << fd;
}

void on_channel_disconnect(SkllChannel &srv, void* data) {
    int fd = *reinterpret_cast<int*>(data);
    srv.console.info("Channel") << "Client disconnected: fd=" << fd;
}

void on_channel_recv(SkllChannel &srv, void* data) {
    std::string msg = reinterpret_cast<std::string*>(data);
    srv.console.debug("Channel") << "Received: " << *msg;
}

void on_channel_error(SkllChannel &srv, void* data) {
    int code = *reinterpret_cast<int*>(data);
    srv.console.error("Channel") << "Error code: " << code;
}
