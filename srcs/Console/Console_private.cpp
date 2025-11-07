/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Console_private.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/07 20:03:06 by marvin            #+#    #+#             */
/*   Updated: 2025/11/07 20:03:06 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <irc.hpp>

		void		_create_directories(const std::string& path);
		int			_level_to_index(e_loglevel lvl) const;
		std::string	_get_timestamp_filename() const;
		std::string	_timestamp() const;
		std::string	_format(int levels, const std::string& msg, const std::string& prefix) const;

void Console::_create_directories(const std::string& path) {
    std::string dir_path = "";
    for (size_t i = 0; i < path.length(); i++) {
        if (path[i] == '/' || path[i] == '\\') {
            if (!dir_path.empty()) mkdir(dir_path.c_str(), 0755);
        }
        dir_path += path[i];
    }
    if (!dir_path.empty() && (dir_path[dir_path.length()-1] == '/' || dir_path[dir_path.length()-1] == '\\')) {
        mkdir(dir_path.c_str(), 0755);
    }
}
		
std::string Console::_get_timestamp_filename() const {
    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);
    
    std::ostringstream ss;
    ss << std::setfill('0')
       << std::setw(4) << (tm_info->tm_year + 1900) << "-"
       << std::setw(2) << (tm_info->tm_mon + 1) << "-"
       << std::setw(2) << tm_info->tm_mday << ";"
       << std::setw(2) << tm_info->tm_hour << ";"
       << std::setw(2) << tm_info->tm_min << ";"
       << std::setw(2) << tm_info->tm_sec;
    
    return (ss.str());
}

std::string Console::_format(int levels, const std::string& msg, const std::string& prefix) const {
    std::ostringstream output;

	if (!_out_stdout) retstd::cout << prefix << ":" << msg << std::endl;
    output << "[" << _timestamp() << "]";
    if (!prefix.empty()) output << " - [" << prefix << "]";    
    for (int i = 0; i < LOG_LEVEL; i++) {
        output << ":[ ";
        if (levels & log_level[i].value)   output << log_level[i].key;
        if (levels & DEBUG)   output << "[DEBUG]";
        if (levels & INFO)    output << "[INFO]";
        if (levels & WARNING) output << "[WARNING]";
        if (levels & ERROR)   output << "[ERROR]";
    }
    return (output << " > " << msg, output.str());
}