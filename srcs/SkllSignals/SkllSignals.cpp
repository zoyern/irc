/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SkllSignals.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/17 02:31:48 by marvin            #+#    #+#             */
/*   Updated: 2025/11/17 02:31:48 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Sockell/SkllSignals.hpp>

int SkllSignals::_pipe[2] = {-1, -1};

void SkllSignals::setup() {
    if (pipe(_pipe) < 0) return;
    
    fcntl(_pipe[0], F_SETFL, O_NONBLOCK);
    fcntl(_pipe[1], F_SETFL, O_NONBLOCK);
    
    struct sigaction sa;
    std::memset(&sa, 0, sizeof(sa));
    sa.sa_handler = &SkllSignals::_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL);
    signal(SIGPIPE, SIG_IGN);
}

void SkllSignals::cleanup() {
    if (_pipe[0] >= 0) close(_pipe[0]);
    if (_pipe[1] >= 0) close(_pipe[1]);
}

void SkllSignals::_handler(int sig) {
    char c = static_cast<char>(sig);
    ssize_t ret = write(_pipe[1], &c, 1);
    (void)ret;
}

int SkllSignals::get_read_fd() {
    return _pipe[0];
}

bool SkllSignals::check() {
    char sig;
    if (read(_pipe[0], &sig, 1) > 0) {
        std::cout << "\n[!] Signal " << (int)sig << " received\n";
        return true;
    }
    return false;
}