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

Console::LogLevel Console::log_level[LOG_LEVEL] = {
	{"LOG",     LOG},
	{"DEBUG",   DEBUG},
	{"INFO",    INFO},
	{"WARNING", WARNING},
	{"ERROR",   ERROR}
};

Console::~Console() { for (int i = 0; i < LOG_LEVEL; i++) { if (_files[i].is_open()) _files[i].close(); }}
Console::Console()
    : _active(true)
    , _out_stdout(OUTPUT_TO_STDOUT)
    , _out_file(OUTPUT_TO_FILE)
	, _enabled_levels(ENABLED_LEVEL)
	, _path_log("")
	, _stream(*this)
{}
Console& Console::instance() { static Console instance; return (instance); }

Console::Stream& Console::log(int level, const std::string& prefix) {
    _stream.reset(level, prefix);
    return _stream;
}

Console::Stream& Console::debug(const std::string& prefix) { return log(DEBUG, prefix); }
Console::Stream& Console::info(const std::string& prefix)  { return log(INFO, prefix); }
Console::Stream& Console::warning(const std::string& prefix) { return log(WARNING, prefix); }
Console::Stream& Console::error(const std::string& prefix) { return log(ERROR, prefix); }


Console::Stream::~Stream() { _console._log_internal(_level, _buffer.str(), _prefix); }
Console::Stream::Stream(Console& console, int level, const std::string& prefix) : _console(console), _level(level), _prefix(prefix) {}

void Console::Stream::reset(int level, const std::string& prefix) {
	_level = level;
	_prefix = prefix;
	_buffer.str("");
	_buffer.clear();
}