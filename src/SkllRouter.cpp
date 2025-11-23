/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SkllRouter.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/21 17:50:19 by marvin            #+#    #+#             */
/*   Updated: 2025/11/21 17:50:19 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Sockell/SkllRouter.hpp>
#include <Sockell/SkllEvent.hpp>
#include <Sockell/SkllMessage.hpp>
#include <Sockell/SkllClient.hpp>
#include <cstring>

SkllRouter::Delim::Delim() : mask(0) {}
SkllRouter::Delim::Delim(int m, const char *delim)
	: mask(m), bytes(delim, delim + std::strlen(delim)) {}

SkllRouter::Handler::Handler()
	: callback(NULL), user_data(NULL) {}
SkllRouter::Handler::Handler(const std::string &p, RouteCallback cb, void *data)
	: pattern(p), callback(cb), user_data(data) {}

SkllRouter::Route::Route() : mask(0) {}
SkllRouter::Route::Route(int m) : mask(m) {}

SkllRouter::SkllRouter()
	: _current_mask(0), _last_handler(NULL) {}
SkllRouter::~SkllRouter() {}

SkllRouter &SkllRouter::ctrl(int mask) {
	_current_mask = mask;
	return *this;
}

SkllRouter &SkllRouter::delim(int mask, const char *delimiter) {
	_delims[mask] = Delim(mask, delimiter);
	return *this;
}

SkllRouter &SkllRouter::route(const char *pattern, RouteCallback callback, void *user_data) {
	Route *route = find_or_create_route(_current_mask);
	route->handlers.push_back(Handler(pattern, callback, user_data));
	_last_handler = &route->handlers.back();
	return *this;
}

SkllRouter &SkllRouter::router() {
    return *this;
}

SkllRouter &SkllRouter::match(const char *pattern, RouteCallback callback, void *user_data) {
	if (_last_handler) {
		_last_handler->children.push_back(Handler(pattern, callback, user_data));
	}
	return *this;
}

SkllRouter::Route *SkllRouter::find_or_create_route(int mask) {
	for (size_t i = 0; i < _routes.size(); ++i) {
		if (_routes[i].mask == mask) {
			return &_routes[i];
		}
	}
	_routes.push_back(Route(mask));
	return &_routes.back();
}

void SkllRouter::on_recv_data(const uint8_t *data, size_t len, SkllEvent *event) {
	_buffer.insert(_buffer.end(), data, data + len);
	
	bool found = true;
	while (found) {
		found = false;
		
		for (size_t i = 0; i < _routes.size(); ++i) {
			const Route &route = _routes[i];
			
			for (std::map<int, Delim>::iterator it = _delims.begin();
				 it != _delims.end(); ++it) {
				
				if ((route.mask  &it->first) != 0) {
					size_t pos = find_delim(it->second.bytes);
					
					if (pos != std::string::npos) {
						found = true;

						if (event->client && event->client->is_rate_limited()) {
							_buffer.erase(_buffer.begin(),
										 _buffer.begin() + pos + it->second.bytes.size());
							break;
						}
						if (event->client) {
							event->client->rate_limit.record();
						}
						
						if (event->message) {
							event->message->clear();
							for (size_t j = 0; j < pos; ++j) {
								*event->message << (char)_buffer[j];
							}
							event->message->rewind();
						}
						
						execute_route(route, event);
						
						_buffer.erase(_buffer.begin(),
									 _buffer.begin() + pos + it->second.bytes.size());
						break;
					}
				}
			}
			if (found) break;
		}
	}
}

size_t SkllRouter::find_delim(const std::vector<uint8_t> &delim) {
	if (delim.empty() || _buffer.size() < delim.size()) {
		return std::string::npos;
	}
	
	for (size_t i = 0; i <= _buffer.size() - delim.size(); ++i) {
		bool match = true;
		for (size_t j = 0; j < delim.size(); ++j) {
			if (_buffer[i + j] != delim[j]) {
				match = false;
				break;
			}
		}
		if (match) return i;
	}
	
	return std::string::npos;
}

bool SkllRouter::execute_route(const Route &route, SkllEvent *event) {
	for (size_t i = 0; i < route.handlers.size(); ++i) {
		if (execute_handler(&route.handlers[i], event)) {
			return true;
		}
	}
	return false;
}

bool SkllRouter::execute_handler(const Handler *h, SkllEvent *event) {
	if (!h || !event->message) return false;
	
	if (!h->pattern.empty()) {
		if (!event->message->starts_with(h->pattern.c_str())) {
			return false;
		}
		event->message->skip(h->pattern.size());
		event->message->skip_spaces();
	}
	
	if (h->callback) {
		h->callback(event, h->user_data);
	}
	
	for (size_t i = 0; i < h->children.size(); ++i) {
		execute_handler(&h->children[i], event);
	}
	
	return true;
}