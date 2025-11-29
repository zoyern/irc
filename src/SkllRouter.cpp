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
#include <Sockell/SkllLog.hpp>
#include <cstring>

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   ROUTER NODE                                                                */
/* ═══════════════════════════════════════════════════════════════════════════ */

SkllRouterNode::SkllRouterNode()
	: var_child(SKLL_NO_NODE), rest_child(SKLL_NO_NODE),
	  var_required(false), var_optional(false), is_endpoint(false) {}

void SkllRouterNode::add_callback(const SkllCallbackHolder &cb) {
	if (cb.valid()) callbacks.push_back(cb);
}

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   ROUTER - NODE POOL MANAGEMENT                                              */
/* ═══════════════════════════════════════════════════════════════════════════ */

/* O(1) amortized - std::deque push_back */
size_t SkllRouter::_alloc_node() {
	_pool.push_back(SkllRouterNode());
	return _pool.size() - 1;
}

SkllRouterNode &SkllRouter::_node(size_t idx) {
	return _pool[idx];
}

const SkllRouterNode &SkllRouter::_node(size_t idx) const {
	return _pool[idx];
}

/* O(log n) - map lookup/insert */
size_t SkllRouter::_get_or_create(size_t parent_idx, const std::string &seg) {
	SkllRouterNode &parent = _node(parent_idx);
	std::map<std::string, size_t>::iterator it = parent.children.find(seg);
	if (it != parent.children.end()) return it->second;
	
	size_t new_idx = _alloc_node();
	_node(new_idx).segment = seg;
	/* Re-get parent reference after potential reallocation (deque is safe but be explicit) */
	_node(parent_idx).children[seg] = new_idx;
	return new_idx;
}

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   ROUTER LIFECYCLE                                                           */
/* ═══════════════════════════════════════════════════════════════════════════ */

SkllRouter::SkllRouter() : _delim(' '), _count(0) {
	_root_idx = _alloc_node();
	_current_idx = _root_idx;
	_stack.push_back(_root_idx);
}

/* No manual delete needed - std::deque destructor handles everything */
SkllRouter::~SkllRouter() {}

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   FLAT API                                                                   */
/* ═══════════════════════════════════════════════════════════════════════════ */

SkllRouter &SkllRouter::on(const std::string &pattern, SkllCallback cb) {
	return _on_holder(pattern, skll_cb(cb));
}

SkllRouter &SkllRouter::_on_holder(const std::string &pattern, const SkllCallbackHolder &cb) {
	route("");
	size_t i = 0, len = pattern.size();
	while (i < len) {
		while (i < len && pattern[i] == ' ') ++i;
		if (i >= len) break;
		
		/* Check for variable patterns */
		if (pattern[i] == '{') {
			size_t end = pattern.find('}', i);
			if (end != std::string::npos) {
				std::string var = pattern.substr(i, end - i + 1);
				size_t child_idx;
				
				/* Check for suffix after '}' (e.g. "{name}.hpp") */
				std::string suffix;
				size_t suffix_end = end + 1;
				while (suffix_end < len && pattern[suffix_end] != ' ' && pattern[suffix_end] != '{') {
					++suffix_end;
				}
				if (suffix_end > end + 1) {
					suffix = pattern.substr(end + 1, suffix_end - end - 1);
				}
				
				/* {*rest} pattern */
				if (var.size() > 2 && var[1] == '*') {
					if (_node(_current_idx).rest_child == SKLL_NO_NODE) {
						_node(_current_idx).rest_child = _alloc_node();
					}
					child_idx = _node(_current_idx).rest_child;
					_node(child_idx).var_name = var.substr(2, var.size() - 3);
				} else {
					if (_node(_current_idx).var_child == SKLL_NO_NODE) {
						_node(_current_idx).var_child = _alloc_node();
					}
					child_idx = _node(_current_idx).var_child;
					
					std::string name = var.substr(1, var.size() - 2);
					if (!name.empty() && name[name.size()-1] == '!') {
						_node(child_idx).var_required = true;
						name = name.substr(0, name.size() - 1);
					} else {
						_node(child_idx).var_optional = true;
					}
					_node(child_idx).var_name = name;
				}
				
				/* Set suffix if present */
				if (!suffix.empty()) {
					_node(child_idx).var_suffix = suffix;
				}
				
				_current_idx = child_idx;
				_stack.push_back(_current_idx);
				i = suffix_end;  /* Skip past suffix */
				continue;
			}
		}
		
		/* Handle prefix pattern like ":{var}" or "int.{var}" */
		if (i + 1 < len && pattern[i] != '{' && pattern[i] != ' ') {
			/* Check if there's a { somewhere ahead in this segment */
			size_t brace_pos = pattern.find('{', i);
			size_t space_pos = pattern.find(' ', i);
			/* Only if brace comes before space (or no space) */
			if (brace_pos != std::string::npos && (space_pos == std::string::npos || brace_pos < space_pos)) {
				size_t end = pattern.find('}', brace_pos);
				if (end != std::string::npos) {
					/* Extract prefix and variable */
					std::string prefix = pattern.substr(i, brace_pos - i);
					std::string var_part = pattern.substr(brace_pos + 1, end - brace_pos - 1);
					
					/* Check for suffix */
					std::string suffix;
					size_t suffix_end = end + 1;
					while (suffix_end < len && pattern[suffix_end] != ' ' && pattern[suffix_end] != '{') {
						++suffix_end;
					}
					if (suffix_end > end + 1) {
						suffix = pattern.substr(end + 1, suffix_end - end - 1);
					}
					
					if (_node(_current_idx).var_child == SKLL_NO_NODE) {
						_node(_current_idx).var_child = _alloc_node();
					}
					size_t child_idx = _node(_current_idx).var_child;
					_node(child_idx).var_prefix = prefix;
					_node(child_idx).var_suffix = suffix;
					
					std::string name = var_part;
					if (!name.empty() && name[name.size()-1] == '!') {
						_node(child_idx).var_required = true;
						name = name.substr(0, name.size() - 1);
					}
					_node(child_idx).var_name = name;
					_current_idx = child_idx;
					_stack.push_back(_current_idx);
					i = suffix_end;
					continue;
				}
			}
		}
		/* Regular segment */
		size_t start = i;
		while (i < len && pattern[i] != ' ' && pattern[i] != '{') ++i;
		if (i > start) {
			std::string seg = pattern.substr(start, i - start);
			_current_idx = _get_or_create(_current_idx, seg);
			_stack.push_back(_current_idx);
		}
	}
	
	/* Add callback to final node */
	_apply_pending(_current_idx);
	_node(_current_idx).add_callback(cb);
	_node(_current_idx).is_endpoint = true;
	++_count;
	
	return *this;
}

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   CASCADE API                                                                */
/* ═══════════════════════════════════════════════════════════════════════════ */

SkllRouter &SkllRouter::route(const std::string &method) {
	_current_idx = _root_idx;
	_stack.clear();
	_stack.push_back(_root_idx);
	_pending.clear();
	
	if (!method.empty()) {
		_current_idx = _get_or_create(_current_idx, method);
		_stack.push_back(_current_idx);
	}
	return *this;
}

SkllRouter &SkllRouter::route(const std::string &method, SkllCallback cb) {
	route(method);
	_node(_current_idx).add_callback(skll_cb(cb));
	_node(_current_idx).is_endpoint = true;
	++_count;
	return *this;
}

SkllRouter &SkllRouter::path(const std::string &segment) {
	size_t target = _parse_segment(segment);
	_current_idx = target;
	_stack.push_back(_current_idx);
	return *this;
}

SkllRouter &SkllRouter::path(const std::string &segment, SkllCallback cb) {
	path(segment);
	_apply_pending(_current_idx);
	_node(_current_idx).add_callback(skll_cb(cb));
	_node(_current_idx).is_endpoint = true;
	++_count;
	if (_stack.size() > 1) { _stack.pop_back(); _current_idx = _stack.back(); }
	return *this;
}

SkllRouter &SkllRouter::node(const std::string &segment, SkllCallback cb) {
	size_t target = segment.empty() ? _current_idx : _parse_segment(segment);
	_apply_pending(target);
	_node(target).add_callback(skll_cb(cb));
	_node(target).is_endpoint = true;
	++_count;
	return *this;
}

SkllRouter &SkllRouter::then(SkllCallback cb) {
	_pending.push_back(skll_cb(cb));
	if (_node(_current_idx).is_endpoint) _node(_current_idx).add_callback(skll_cb(cb));
	return *this;
}

SkllRouter &SkllRouter::fallback(SkllCallback cb) {
	_fallback = skll_cb(cb);
	return *this;
}

size_t SkllRouter::_parse_segment(const std::string &seg) {
	if (seg.empty()) return _current_idx;
	
	std::string prefix, var_name, suffix;
	bool is_var = false, is_rest = false, is_required = false;
	
	/* Parse segment for variable patterns */
	size_t brace = seg.find('{');
	if (brace != std::string::npos) {
		size_t end_brace = seg.find('}', brace);
		if (end_brace != std::string::npos) {
			is_var = true;
			prefix = seg.substr(0, brace);
			suffix = seg.substr(end_brace + 1);
			std::string var_part = seg.substr(brace + 1, end_brace - brace - 1);
			
			if (!var_part.empty() && var_part[0] == '*') {
				is_rest = true;
				var_name = var_part.substr(1);
			} else {
				if (!var_part.empty() && var_part[var_part.size()-1] == '!') {
					is_required = true;
					var_name = var_part.substr(0, var_part.size() - 1);
				} else {
					var_name = var_part;
				}
			}
		}
	}
	
	if (is_var) {
		size_t child_idx;
		if (is_rest) {
			if (_node(_current_idx).rest_child == SKLL_NO_NODE) {
				_node(_current_idx).rest_child = _alloc_node();
			}
			child_idx = _node(_current_idx).rest_child;
		} else {
			if (_node(_current_idx).var_child == SKLL_NO_NODE) {
				_node(_current_idx).var_child = _alloc_node();
			}
			child_idx = _node(_current_idx).var_child;
		}
		
		SkllRouterNode &child = _node(child_idx);
		child.var_name = var_name;
		child.var_prefix = prefix;
		child.var_suffix = suffix;
		child.var_required = is_required;
		child.var_optional = !is_required && !is_rest;
		return child_idx;
	}
	
	return _get_or_create(_current_idx, seg);
}

void SkllRouter::_apply_pending(size_t node_idx) {
	for (size_t i = 0; i < _pending.size(); ++i)
		_node(node_idx).add_callback(_pending[i]);
}

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   DISPATCH - ITERATIVE VERSION (NO RECURSION, NO STACK OVERFLOW)            */
/*   O(k *log c) where k=segments, c=children per node                        */
/* ═══════════════════════════════════════════════════════════════════════════ */

int SkllRouter::dispatch(SkllEvent &event) {
	SkllSpan raw = event.message().raw();
	const char *data = reinterpret_cast<const char*>(raw.data);
	size_t len = raw.len;
	const char *end = data + len;
	const char *pos = data;
	
	size_t node_idx = _root_idx;
	
	/* Iterative trie traversal - NO RECURSION */
	while (true) {
		const SkllRouterNode &node = _node(node_idx);
		
		/* Skip leading whitespace/delimiter */
		while (pos < end && (*pos == ' ' || *pos == _delim)) ++pos;
		
		/* Find segment end */
		const char *seg_start = pos;
		while (pos < end && *pos != ' ' && *pos != _delim) ++pos;
		size_t seg_len = pos - seg_start;
		
		/* At end of input - check if this node is an endpoint */
		if (seg_len == 0) {
			if (node.is_endpoint) {
				for (size_t i = 0; i < node.callbacks.size(); ++i) {
					int ret = node.callbacks[i].invoke(event);
					if (ret == SKLL_SKIP) break;
					if (ret == SKLL_STOP) return SKLL_STOP;
					if (ret < 0) return ret;
				}
				return SKLL_OK;
			}
		}
		
		std::string seg(seg_start, seg_len);
		
		/* Try exact match first - O(log n) */
		std::map<std::string, size_t>::const_iterator it = node.children.find(seg);
		if (it != node.children.end()) {
			node_idx = it->second;
			continue;  /* Next iteration - no recursion! */
		}
		
		/* Try variable child */
		if (node.var_child != SKLL_NO_NODE) {
			const SkllRouterNode &var_node = _node(node.var_child);
			std::string val = seg;
			bool var_match = true;
			
			/* Check prefix */
			if (!var_node.var_prefix.empty()) {
				if (seg.size() >= var_node.var_prefix.size() &&
					seg.substr(0, var_node.var_prefix.size()) == var_node.var_prefix) {
					val = seg.substr(var_node.var_prefix.size());
				} else {
					var_match = false;
				}
			}
			
			/* Check suffix */
			if (var_match && !var_node.var_suffix.empty()) {
				if (val.size() >= var_node.var_suffix.size() &&
					val.substr(val.size() - var_node.var_suffix.size()) == var_node.var_suffix) {
					val = val.substr(0, val.size() - var_node.var_suffix.size());
				} else {
					var_match = false;
				}
			}
			
			/* Check required */
			if (var_match && var_node.var_required && val.empty()) {
				var_match = false;
			}
			
			if (var_match) {
				/* Store variable */
				if (!var_node.var_name.empty()) {
					if (!var_node.var_prefix.empty() || !var_node.var_suffix.empty()) {
						event.message().set_param(var_node.var_name, val);
					} else {
						event.message().set_param(var_node.var_name, SkllSpan(reinterpret_cast<const byte_t*>(seg_start), seg_len));
					}
				}
				node_idx = node.var_child;
				continue;  /* Next iteration - no recursion! */
			}
		}
		
		/* Try rest capture */
		if (node.rest_child != SKLL_NO_NODE) {
			const SkllRouterNode &rest_node = _node(node.rest_child);
			
			/* Capture everything from current segment */
			const char *rest_start = seg_start;
			while (rest_start < end && (*rest_start == ' ' || *rest_start == _delim)) ++rest_start;
			size_t rest_len = 0;
			if (rest_start < end) {
				const char *rest_end = end;
				/* Trim trailing whitespace */
				while (rest_end > rest_start && (*(rest_end-1) == ' ' || *(rest_end-1) == '\r' || *(rest_end-1) == '\n'))
					--rest_end;
				rest_len = rest_end - rest_start;
			}
			
			if (!rest_node.var_name.empty()) {
				event.message().set_param(rest_node.var_name, SkllSpan(reinterpret_cast<const byte_t*>(rest_start), rest_len));
			}
			
			/* Rest node is always endpoint */
			if (rest_node.is_endpoint) {
				for (size_t i = 0; i < rest_node.callbacks.size(); ++i) {
					int ret = rest_node.callbacks[i].invoke(event);
					if (ret == SKLL_SKIP) break;
					if (ret == SKLL_STOP) return  SKLL_STOP;
					if (ret < 0) return ret;
				}
				return SKLL_OK;
			}
		}
		
		/* If no children matched but this node is an endpoint, invoke callbacks */
		/* This allows patterns like "PASS" to match "PASS arg1 arg2..." */
		if (node.is_endpoint) {
			for (size_t i = 0; i < node.callbacks.size(); ++i) {
				int ret = node.callbacks[i].invoke(event);
				if (ret == SKLL_SKIP) break;
				if (ret == SKLL_STOP) return SKLL_STOP;
				if (ret < 0) return ret;
			}
			return SKLL_OK;
		}
		break;
	}
	
	if (_fallback.valid()) {
		return _fallback.invoke(event);
	}
	return SKLL_OK;
}

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   CONFIGURATION                                                             */
/* ═══════════════════════════════════════════════════════════════════════════ */

SkllRouter &SkllRouter::set_delim(char d) { _delim = d; return *this; }
char SkllRouter::delim() const { return _delim; }
size_t SkllRouter::count() const { return _count; }

void SkllRouter::clear() {
	_pool.clear();
	_root_idx = _alloc_node();
	_current_idx = _root_idx;
	_stack.clear();
	_stack.push_back(_root_idx);
	_pending.clear();
	_fallback = SkllCallbackHolder();
	_count = 0;
}
