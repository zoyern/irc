/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SkllRouter.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/21 17:49:23 by marvin            #+#    #+#             */
/*   Updated: 2025/11/21 17:49:23 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <string>
#include <vector>
#include <map>
#include <stdint.h>
#include <cstddef>

struct SkllEvent;

class SkllRouter {
public:
	struct Delim {
		int mask;
		std::vector<uint8_t> bytes;
		
		Delim();
		Delim(int m, const char *delim);
	};
	
	typedef void (*RouteCallback)(SkllEvent *event, void *user_data);
	
	struct Handler {
		std::string pattern;
		RouteCallback callback;
		void *user_data;
		std::vector<Handler> children;
		
		Handler();
		Handler(const std::string& p, RouteCallback cb, void *data);
	};
	
	struct Route {
		int mask;
		std::vector<Handler> handlers;
		
		Route();
		Route(int m);
	};

private:
	std::map<int, Delim> _delims;
	std::vector<Route> _routes;
	std::vector<uint8_t> _buffer;
	
	int _current_mask;
	Handler *_last_handler;

public:
	SkllRouter();
	~SkllRouter();
	
	SkllRouter &ctrl(int mask);
	SkllRouter &delim(int mask, const char *delimiter);
	SkllRouter &route(const char *pattern, RouteCallback callback, void *user_data = NULL);
	SkllRouter &match(const char *pattern, RouteCallback callback, void *user_data = NULL);
	SkllRouter &router();
	
	void on_recv_data(const uint8_t *data, size_t len, SkllEvent *event);
	
private:
	size_t find_delim(const std::vector<uint8_t> &delim);
	bool execute_route(const Route &route, SkllEvent *event);
	bool execute_handler(const Handler *h, SkllEvent *event);
	Route *find_or_create_route(int mask);
	
	SkllRouter(const SkllRouter&);
	SkllRouter &operator=(const SkllRouter&);
};