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

#include <Sockell.hpp>

void Console::_create_directories(const std::string& path) {
    if (path.empty()) return;
    
    std::string dir_path = "";
    for (size_t i = 0; i < path.length(); i++) {
        dir_path += path[i];
        if (path[i] == '/' || path[i] == '\\') {
            if (!dir_path.empty())
                mkdir(dir_path.c_str(), 0755);
        }
    }
    if (!dir_path.empty() && 
        (dir_path[dir_path.length()-1] == '/' || dir_path[dir_path.length()-1] == '\\')) {
        mkdir(dir_path.c_str(), 0755);
    }
}

void Console::_log_internal(int levels, const std::string& msg, const std::string& prefix) {
    if (!_active) return;
    
    std::string output = _format(levels, msg, prefix);
    if (_out_stdout && (_enabled_levels & levels)) 
        levels & ERROR ? std::cerr << output << std::endl : std::cout << output << std::endl;
    
    if (!_out_file) return;
    if (_files[0].is_open()) {
        _files[0] << output << std::endl;
        _files[0].flush();
    }
    for (int i = 1; i < LOG_LEVEL; i++) {
        if ((levels & log_level[i].value) && _files[i].is_open()) {
            _files[i] << output << std::endl;
            _files[i].flush();
        }
    }
}

std::string Console::_get_timestamp_filename() const {
    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);
    
    std::ostringstream ss;
    ss << std::setfill('0')
       << std::setw(4) << (tm_info->tm_year + 1900) << "-"
       << std::setw(2) << (tm_info->tm_mon + 1) << "-"
       << std::setw(2) << tm_info->tm_mday << "_"
       << std::setw(2) << tm_info->tm_hour << "-"
       << std::setw(2) << tm_info->tm_min << "-"
       << std::setw(2) << tm_info->tm_sec;
    return ss.str();
}


std::string Console::_timestamp() const {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    
    time_t now = tv.tv_sec;
    struct tm* tm_info = localtime(&now);
    
    std::ostringstream ss;
    ss << std::setfill('0')
       << std::setw(4) << (tm_info->tm_year + 1900) << "-"
       << std::setw(2) << (tm_info->tm_mon + 1) << "-"
       << std::setw(2) << tm_info->tm_mday << " "
       << std::setw(2) << tm_info->tm_hour << ":"
       << std::setw(2) << tm_info->tm_min << ":"
       << std::setw(2) << tm_info->tm_sec << "."
       << std::setw(3) << (tv.tv_usec / 1000);
    return ss.str();
}

std::string Console::_format(int levels, const std::string& msg, const std::string& prefix) const {
    std::ostringstream output;
    
    output << "[" << _timestamp() << "]";
    if (!prefix.empty()) output << " - [" << prefix << "]";
    
    if (levels != NONE) {
        output << ":[";
        bool first = true;
        for (int i = 0; i < LOG_LEVEL; i++) {
            if (levels & log_level[i].value) {
                if (!first)
                    output << " / ";
                output << log_level[i].key;
                first = false;
            }
        }
        output << "]";
    }
    output << " > " << msg;
    return (output.str());
}