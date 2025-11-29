/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Hook.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/09 00:50:10 by marvin            #+#    #+#             */
/*   Updated: 2025/11/09 00:50:10 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include <Sockell/SkllHook.hpp>
#include <Sockell/SkllEvent.hpp>
#include <iostream>

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   CALLBACK HOLDER                                                            */
/* ═══════════════════════════════════════════════════════════════════════════ */

SkllCallbackHolder::SkllCallbackHolder() : fn_ptr(0), user_data(0), invoker(0) {}

int SkllCallbackHolder::invoke(SkllEvent &e) const {
	if (!invoker || !fn_ptr) return SKLL_OK;;
	return invoker(fn_ptr, e, user_data);
}

bool SkllCallbackHolder::valid() const {
	return fn_ptr != 0 && invoker != 0;
}

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   INVOKERS                                                                   */
/* ═══════════════════════════════════════════════════════════════════════════ */

int skll_invoke_simple(void *fn, SkllEvent &e, void *) {
	typedef int (*SimpleFn)(SkllEvent &);
	SimpleFn simple_fn = reinterpret_cast<SimpleFn>(fn);
	return simple_fn(e);
}

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   CALLBACK CREATION                                                          */
/* ═══════════════════════════════════════════════════════════════════════════ */

SkllCallbackHolder skll_cb(int (*fn)(SkllEvent &)) {
	SkllCallbackHolder h;
	h.fn_ptr = reinterpret_cast<void *>(fn);
	h.user_data = 0;
	h.invoker = &skll_invoke_simple;
	return h;
}

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   HOOK                                                                       */
/* ═══════════════════════════════════════════════════════════════════════════ */

SkllHook::SkllHook() {}

SkllHook::SkllHook(SkllCallback fn) : holder(skll_cb(fn)) {}

SkllHook::SkllHook(const SkllCallbackHolder &h) : holder(h) {}

int SkllHook::call(SkllEvent &e) const {
	return holder.invoke(e);
}

bool SkllHook::valid() const {
	return holder.valid();
}

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   HOOKS                                                                      */
/* ═══════════════════════════════════════════════════════════════════════════ */

SkllHooks::SkllHooks() {}
SkllHooks::~SkllHooks() {}

SkllHooks &SkllHooks::add(SkllCallback fn) {
	if (fn) _hooks.push_back(SkllHook(fn));
	return *this;
}

SkllHooks &SkllHooks::add(const SkllCallbackHolder &h) {
	if (h.valid()) _hooks.push_back(SkllHook(h));
	return *this;
}

void SkllHooks::clear() { _hooks.clear(); }
bool SkllHooks::empty() const { return _hooks.empty(); }
size_t SkllHooks::size() const { return _hooks.size(); }

int SkllHooks::run(SkllEvent &e) const {
	for (size_t i = 0; i < _hooks.size(); ++i) {
		int ret = _hooks[i].call(e);
		if (ret == SKLL_STOP || ret == SKLL_FATAL || ret == SKLL_ERR)
			return ret;
		if (ret == SKLL_SKIP)
			break;
	}
	return SKLL_OK;
}


const SkllHook &SkllHooks::at(size_t i) const { return _hooks[i]; }
