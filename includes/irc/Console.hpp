/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Console.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/07 16:56:09 by marvin            #+#    #+#             */
/*   Updated: 2025/11/07 16:56:09 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <irc.hpp>

enum e_loglevel {
    NONE    = 0,
    LOG   	= 1 << 0,
    DEBUG   = 1 << 1,
    INFO    = 1 << 2,
    WARNING = 1 << 3,
    ERROR   = 1 << 4,
    ALL     = LOG | DEBUG | INFO | WARNING | ERROR
};



#ifndef CONSOLE_SETTINGS
    #define ENABLED_LEVEL		(DEBUG | INFO | WARNING | ERROR)
	#define OUTPUT_TO_STDOUT	true
    #define OUTPUT_TO_FILE		false
	#define LOG_LEVEL			5
	#define ERROR_LOG_OPEN		"[Console] Failed to open log file: "
	#define ERROR_LOGS_OPEN		"[Console] Failed to create any log files!"
#endif

class Console {
	public:
		struct LogLevel { std::string key; e_loglevel value; };
		static LogLevel	log_level[LOG_LEVEL];
		class Stream {
			private:
				Console            &_console;
				int                 _level;
				std::string         _prefix;
				std::ostringstream  _buffer;
			
			public:
				~Stream();
				Stream(Console& console, int level = LOG, const std::string& prefix = "");

				void reset(int level, const std::string& prefix);
				template<typename T>
				Stream& operator<<(const T& value);

		};
	private:
		bool			_active;
		bool 			_out_stdout;
		bool			_out_file;
		int				_enabled_levels;
		std::string		_path_log;
		Stream			_stream;
		std::ofstream   _files[LOG_LEVEL];

		Console();

	void		_create_directories(const std::string& path);
	void		_log_internal(int levels, const std::string& msg, const std::string& prefix);
    std::string	_get_timestamp_filename() const;
    std::string	_timestamp() const;
    std::string	_format(int levels, const std::string& msg, const std::string& prefix) const;
	public:
		~Console();

		static Console& instance();

		Console&	enable(e_loglevel levels);
		Console&	disable(e_loglevel levels);
		Console&	to_stdout(bool enable);
		Console&	to_file(const std::string& path);

		int		get_file_count() const;
		bool	is_active() const;

		void	start();
		void	stop();

		Stream &log(int level = LOG, const std::string& prefix = "");
		Stream &debug(const std::string& prefix = "");
		Stream &info(const std::string& prefix = "");
		Stream &warning(const std::string& prefix = "");
		Stream &error(const std::string& prefix = "");
};

#include <irc/Console/Stream.tpp>