/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SkllConsole_public.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/07 20:06:23 by marvin            #+#    #+#             */
/*   Updated: 2025/11/07 20:06:23 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Sockell/SkllConsole.hpp>

SkllConsole& SkllConsole::enable(e_loglevel levels) {_enabled_levels |= static_cast<int>(levels); return *this; }
SkllConsole& SkllConsole::disable(e_loglevel levels) {_enabled_levels &= ~static_cast<int>(levels); return *this; }
SkllConsole& SkllConsole::to_stdout(bool enable) { _out_stdout = enable; return *this; }
SkllConsole& SkllConsole::to_file(const std::string& path) {
    if (!_active) return *this;
    
    size_t last_slash = path.find_last_of("/\\");
    std::string dir = (last_slash != std::string::npos) ? path.substr(0, last_slash + 1) : "";
    std::string base = (last_slash != std::string::npos) ? path.substr(last_slash + 1) : path;
    
    if (!dir.empty()) _create_directories(dir);
    
    std::string timestamp = _get_timestamp_filename();
    
    bool success = false;

    for (int i = 0; i < LOG_LEVEL; i++) {
        if (_files[i].is_open())
            _files[i].close();

        std::ostringstream filename;
        filename << dir << base << "_" << log_level[i].key << "-" << timestamp << ".txt";
        
        _files[i].open(filename.str().c_str(), std::ios::out | std::ios::app);
        if (_files[i].is_open()) {
            success = true;
        } else {
            std::cerr << ERROR_LOG_OPEN << filename.str() << std::endl;
        }
    }

    if (!success) {
        std::cerr << ERROR_LOGS_OPEN << std::endl;
        for (int i = 0; i < LOG_LEVEL; i++) {
            if (_files[i].is_open())
                _files[i].close();
        }
        return *this;
    }

    _out_file = true;
    _path_log = path;
    log(INFO, "Console") << "Log files created: " << dir << base << "_*-" << timestamp << ".txt";
    return *this;
}

int SkllConsole::get_file_count() const {
    if (!_out_file) return (0);
    int count = 0;
    for (int i = 0; i < LOG_LEVEL; i++) {if (_files[i].is_open()) count++;}
    return (count);
}

bool SkllConsole::is_active() const { return _active; }
void SkllConsole::start() { _active = true; }
void SkllConsole::stop() { _active = false; }

