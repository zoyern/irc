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

#include <Sockell/SkllTypes.hpp>
#include <Sockell/SkllHook.hpp>
#include <string>
#include <vector>
#include <map>
#include <deque>

class SkllEvent;

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   CONSTANTS                                                                 */
/* ═══════════════════════════════════════════════════════════════════════════ */

#define SKLL_NO_NODE ((size_t)-1)

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   ROUTER NODE - Trie node for hierarchical routing                          */
/*   NO MANUAL MEMORY - Stored in pool managed by SkllRouter                   */
/* ═══════════════════════════════════════════════════════════════════════════ */

struct SkllRouterNode {
	std::string								segment;
	std::map<std::string, size_t>			children;		/* Index into pool O(log n) */
	size_t									var_child;		/* Index (SKLL_NO_NODE = none) */
	size_t									rest_child;		/* Index (SKLL_NO_NODE = none) */
	std::string								var_name;
	std::string								var_prefix;
	std::string								var_suffix;
	bool									var_required;
	bool									var_optional;
	std::vector<SkllCallbackHolder>			callbacks;
	bool									is_endpoint;

	SkllRouterNode();
	void add_callback(const SkllCallbackHolder &cb);
};

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   ROUTER V6 - Zero-allocation trie router with node pool                    */
/*                                                                             */
/*   MEMORY: All nodes in std::deque<SkllRouterNode> (STL manages memory)      */
/*   NO new/delete - PURE STL                                                  */
/*   COMPLEXITY:                                                               */
/*     - Route registration: O(k *log c) where k=segments, c=children         */
/*     - Dispatch: O(k *log c) - trie traversal with map lookup               */
/*     - Memory: O(n) where n = total nodes                                    */
/*                                                                             */
/*   TYPED CALLBACKS (NO CAST NEEDED!):                                        */
/*     int handler(SkllEvent& e, MyServer* srv) { ... }                        */
/*     router.on("CMD {var}", handler, &my_server);                            */
/*                                                                             */
/*   VARIABLES:                                                                */
/*     {var}      Optional variable                                            */
/*     {var!}     Required variable                                            */
/*     :{var}     Prefixed variable                                            */
/*     {var}.ext  Suffixed variable                                            */
/*     {*rest}    Captures rest of message                                     */
/*                                                                             */
/*   RETURN CODES:                                                             */
/*     SKLL_OK    (0)  - Success, continue                                     */
/*     SKLL_NEXT  (1)  - Try next route                                        */
/*     SKLL_SKIP  (2)  - Skip remaining callbacks                              */
/*     SKLL_STOP  (3)  - Stop all routing                                      */
/*     SKLL_ERR   (-1) - Error                                                 */
/*     SKLL_FATAL (-2) - Fatal error                                           */
/* ═══════════════════════════════════════════════════════════════════════════ */

class SkllRouter {
public:
	SkllRouter();
	~SkllRouter();

	/* ═══════════════════════════════════════════════════════════════════════ */
	/*   FLAT API                                                               */
	/* ═══════════════════════════════════════════════════════════════════════ */

	SkllRouter &	on(const std::string &pattern, SkllCallback cb);

	template<typename T>
	SkllRouter &	on(const std::string &pattern, int (*cb)(SkllEvent &, T *), T *data) {
		return _on_holder(pattern, skll_cb(cb, data));
	}

	/* ═══════════════════════════════════════════════════════════════════════ */
	/*   CASCADE API                                                            */
	/* ═══════════════════════════════════════════════════════════════════════ */

	SkllRouter &	route(const std::string &method);
	SkllRouter &	route(const std::string &method, SkllCallback cb);
	template<typename T>
	SkllRouter &	route(const std::string &method, int (*cb)(SkllEvent &, T *), T *data) {
		route(method);
		_node(_current_idx).add_callback(skll_cb(cb, data));
		_node(_current_idx).is_endpoint = true;
		++_count;
		return *this;
	}

	SkllRouter &	path(const std::string &segment);
	SkllRouter &	path(const std::string &segment, SkllCallback cb);
	template<typename T>
	SkllRouter &	path(const std::string &segment, int (*cb)(SkllEvent &, T *), T *data) {
		path(segment);
		_apply_pending(_current_idx);
		_node(_current_idx).add_callback(skll_cb(cb, data));
		_node(_current_idx).is_endpoint = true;
		++_count;
		if (_stack.size() > 1) { _stack.pop_back(); _current_idx = _stack.back(); }
		return *this;
	}

	SkllRouter &	node(const std::string &segment, SkllCallback cb);
	template<typename T>
	SkllRouter &	node(const std::string &segment, int (*cb)(SkllEvent &, T *), T *data) {
		size_t target = segment.empty() ? _current_idx : _parse_segment(segment);
		_apply_pending(target);
		_node(target).add_callback(skll_cb(cb, data));
		_node(target).is_endpoint = true;
		++_count;
		return *this;
	}

	SkllRouter &	then(SkllCallback cb);
	template<typename T>
	SkllRouter &	then(int (*cb)(SkllEvent &, T *), T *data) {
		_pending.push_back(skll_cb(cb, data));
		if (_node(_current_idx).is_endpoint) _node(_current_idx).add_callback(skll_cb(cb, data));
		return *this;
	}

	SkllRouter &	fallback(SkllCallback cb);
	template<typename T>
	SkllRouter &	fallback(int (*cb)(SkllEvent &, T *), T *data) {
		_fallback = skll_cb(cb, data);
		return *this;
	}

	/* ═══════════════════════════════════════════════════════════════════════ */
	/*   DISPATCH                                                               */
	/* ═══════════════════════════════════════════════════════════════════════ */

	int				dispatch(SkllEvent &event);

	/* ═══════════════════════════════════════════════════════════════════════ */
	/*   CONFIGURATION                                                          */
	/* ═══════════════════════════════════════════════════════════════════════ */

	SkllRouter &	set_delim(char d);
	char			delim() const;
	size_t			count() const;
	void			clear();

private:
	/* NODE POOL - STL manages all memory */
	std::deque<SkllRouterNode>		_pool;
	size_t							_root_idx;
	size_t							_current_idx;
	std::vector<size_t>				_stack;
	std::vector<SkllCallbackHolder>	_pending;
	SkllCallbackHolder				_fallback;
	char							_delim;
	size_t							_count;

	/* Pool management - O(1) allocation */
	size_t				_alloc_node();
	SkllRouterNode &	_node(size_t idx);
	const SkllRouterNode &_node(size_t idx) const;
	size_t				_get_or_create(size_t parent_idx, const std::string &seg);

	/* Internal */
	SkllRouter &		_on_holder(const std::string &pattern, const SkllCallbackHolder &cb);
	size_t				_parse_segment(const std::string &seg);
	void				_apply_pending(size_t node_idx);

	SkllRouter(const SkllRouter &);
	SkllRouter &operator=(const SkllRouter &);
};
