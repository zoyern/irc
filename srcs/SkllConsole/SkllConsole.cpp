/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SkllConsole.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/07 17:24:20 by marvin            #+#    #+#             */
/*   Updated: 2025/11/07 17:24:20 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Sockell/SkllConsole.hpp>

SkllConsole::LogLevel SkllConsole::log_level[LOG_LEVEL] = {
	{"LOG",     LOG},
	{"DEBUG",   DEBUG},
	{"INFO",    INFO},
	{"WARNING", WARNING},
	{"ERROR",   ERROR}
};

SkllConsole::~SkllConsole() { for (int i = 0; i < LOG_LEVEL; i++) { if (_files[i].is_open()) _files[i].close(); }}
SkllConsole::SkllConsole()
    : _active(true)
    , _out_stdout(OUTPUT_TO_STDOUT)
    , _out_file(OUTPUT_TO_FILE)
	, _enabled_levels(ENABLED_LEVEL)
	, _path_log("")
	, _stream(*this)
{}
SkllConsole& SkllConsole::instance() { static SkllConsole instance; return (instance); }

SkllConsole::Stream& SkllConsole::log(int level, const std::string& prefix) {
    _stream.reset(level, prefix);
    return _stream;
}

SkllConsole::Stream& SkllConsole::debug(const std::string& prefix) { return log(DEBUG, prefix); }
SkllConsole::Stream& SkllConsole::info(const std::string& prefix)  { return log(INFO, prefix); }
SkllConsole::Stream& SkllConsole::warning(const std::string& prefix) { return log(WARNING, prefix); }
SkllConsole::Stream& SkllConsole::error(const std::string& prefix) { return log(ERROR, prefix); }


SkllConsole::Stream::~Stream() { _console._log_internal(_level, _buffer.str(), _prefix); }
SkllConsole::Stream::Stream(SkllConsole& console, int level, const std::string& prefix) : _console(console), _level(level), _prefix(prefix) {}

void SkllConsole::Stream::reset(int level, const std::string& prefix) {
	_level = level;
	_prefix = prefix;
	_buffer.str("");
	_buffer.clear();
}