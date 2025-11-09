/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hook_SkllServer.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/09 01:32:23 by marvin            #+#    #+#             */
/*   Updated: 2025/11/09 01:32:23 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Sockell.hpp>

void on_start(SkllServer &srv, void* data) {
    (void)data;
    srv.console.info("SkllServer") << "SkllServer started on " << srv.address() << ":" << srv.port();
}

void on_update(SkllServer &srv, void* data) {
    (void)srv;
	(void)data;
}

void on_shutdown(SkllServer &srv, void* data) {
    (void)data;
    srv.console.info("SkllServer") << "SkllServer shutting down.";
}

void on_connect(SkllServer &srv, void* data) {
    int fd = *reinterpret_cast<int*>(data);
    srv.console.info("SkllServer") << "Client connected: fd=" << fd;
}

void on_disconnect(SkllServer &srv, void* data) {
    int fd = *reinterpret_cast<int*>(data);
    srv.console.info("SkllServer") << "Client disconnected: fd=" << fd;
}

void on_recv(SkllServer &srv, void* data) {
    std::string msg = reinterpret_cast<std::string*>(data);
    srv.console.debug("SkllServer") << "Received: " << *msg;
}

void on_error(SkllServer &srv, void* data) {
    int code = *reinterpret_cast<int*>(data);
    srv.console.error("SkllServer") << "Error code: " << code;
}
