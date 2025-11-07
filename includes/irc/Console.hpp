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

//bitflags pour combiner les niveaux de log

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
    #define OUTPUT_TO_FILE		false
    #define OUTPUT_TO_STDOUT	true
	#define LOG_LEVEL			5

	#define ERROR_LOG_OPEN		"[Console] Failed to open log file: "
	#define ERROR_LOGS_OPEN		"[Console] Failed to create any log files!"
#endif

class Console {
	public:
		typedef static struct {std::string key; e_loglevel value;} log_level[LOG_LEVEL];
	private:
		bool			_active;
		bool 			_out_stdout;
		bool			_out_file;
		int				_enabled_levels;
		std::string		_path_log;
		std::ofstream   _files[LOG_LEVEL];

		Console();

		void		_create_directories(const std::string& path);
		int			_level_to_index(e_loglevel lvl) const;
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

		void	log(int levels, const std::string& msg, const std::string& prefix = "");
		void	log(const std::string& msg);

		void	debug(const std::string& msg, const std::string& prefix = "");
		void	info(const std::string& msg, const std::string& prefix = "");
		void	warning(const std::string& msg, const std::string& prefix = "");
		void	error(const std::string& msg, const std::string& prefix = "");

	private:
};