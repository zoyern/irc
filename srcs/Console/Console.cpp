/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Console.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/07 17:24:20 by marvin            #+#    #+#             */
/*   Updated: 2025/11/07 17:24:20 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <irc.hpp>

struct {std::string key; e_loglevel value;} Console::log_level[LOG_LEVEL] = {
	{"LOG", LOG},
	{"DEBUG", DEBUG},
	{"INFO", DEBUG},
	{"WARNING", DEBUG},
	{"ERROR", DEBUG},
	{"", NONE}
};

Console::~Console() { for (int i = 0; i < LOG_LEVEL; i++) { if (_files[i].is_open()) _files[i].close(); }}

Console::Console()
    : _active(true)
    , _enabled_levels(ENABLED_LEVEL)
    , _output_to_stdout(OUTPUT_TO_STDOUT)
    , _output_to_file(OUTPUT_TO_FILE)
{}

void Console::log(int levels, const std::string& msg, const std::string& prefix) {
    if (!_active) return;

    std::ostringstream output(_format(levels, msg, prefix));
    
    if (_output_to_file) {
        if (_files[4].is_open()) {
            _files[4] << output << std::endl;
            _files[4].flush();
        }
        
        // Écrire dans fichiers spécifiques
        if (levels & DEBUG) {
            int idx = _level_to_index(DEBUG);
            if (idx >= 0 && _files[idx].is_open()) {
                _files[idx] << output << std::endl;
                _files[idx].flush();
            }
        }
        if (levels & INFO) {
            int idx = _level_to_index(INFO);
            if (idx >= 0 && _files[idx].is_open()) {
                _files[idx] << output << std::endl;
                _files[idx].flush();
            }
        }
        if (levels & WARNING) {
            int idx = _level_to_index(WARNING);
            if (idx >= 0 && _files[idx].is_open()) {
                _files[idx] << output << std::endl;
                _files[idx].flush();
            }
        }
        if (levels & ERROR) {
            int idx = _level_to_index(ERROR);
            if (idx >= 0 && _files[idx].is_open()) {
                _files[idx] << output << std::endl;
                _files[idx].flush();
            }
        }
    }
}

void Console::debug(const std::string& msg, const std::string& prefix) {log(DEBUG, msg, prefix); }
void Console::info(const std::string& msg, const std::string& prefix) {log(INFO, msg, prefix); }
void Console::warning(const std::string& msg, const std::string& prefix) { log(WARNING, msg, prefix); }
void Console::error(const std::string& msg, const std::string& prefix) { log(ERROR, msg, prefix); }
