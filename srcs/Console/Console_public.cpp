/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Console_public.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/07 20:06:23 by marvin            #+#    #+#             */
/*   Updated: 2025/11/07 20:06:23 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <irc.hpp>

Console& Console::enable(e_loglevel levels) {_enabled_levels |= static_cast<int>(levels); return *this; }
Console& Console::disable(e_loglevel levels) {_enabled_levels &= ~static_cast<int>(levels); return *this; }
Console& Console::to_stdout(bool enable) { _out_stdout = enable; return *this; }
Console& Console::to_file(const std::string& path) {

	std::ofstream   files[LOG_LEVEL];
    size_t last_slash = path.find_last_of("/\\");
    std::string dir = "";
    std::string base = path;
    
    if (last_slash != std::string::npos) {
        dir = path.substr(0, last_slash + 1);
        base = path.substr(last_slash + 1);
    }
    if (!dir.empty()) { _create_directories(dir); }

	std::string timestamp = _get_timestamp_filename();
	_out_file  = false;
	for (int i = 0; i < LOG_LEVEL; i++) {
		std::ostringstream filename;
		filename << dir << base << "_" << log_level[i] << "-" << timestamp << ".txt";
		file[i].open(filename.str().c_str(), std::ios::out | std::ios::app);
		files[i].is_open() ? _out_file  = true : std::cerr << ERROR_LOG_OPEN << filename.str() << std::endl;
	}
    
    if (!_out_file)
		return (std::cerr << ERROR_LOGS_OPEN << std::endl, !_out_file, *this);
	
    _path_log = path;
	_files = files;
	std::ostringstream msg;
	msg << "Log files created: " << dir << base << "_*-" << timestamp << ".txt";
	log(INFO, msg.str(), "Console");
    return (*this);
}