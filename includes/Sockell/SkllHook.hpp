/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SkllHook.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/09 01:23:17 by marvin            #+#    #+#             */
/*   Updated: 2025/11/09 01:23:17 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#pragma once

#include <Sockell/SkllTypes.hpp>
#include <deque>

class SkllEvent;

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   CALLBACK HOLDER - Type-erased callback with user data                     */
/*                                                                             */
/*   USAGE (NO CAST NEEDED!):                                                  */
/*     int my_handler(SkllEvent& e, IRCServer* irc) { ... }                    */
/*     router.on("CMD", my_handler, &my_irc_server);                           */
/*                                                                             */
/*   The user's callback receives the TYPED pointer directly!                  */
/* ═══════════════════════════════════════════════════════════════════════════ */

struct SkllCallbackHolder {
	void *	fn_ptr;
	void *	user_data;
	int		(*invoker)(void *, SkllEvent &, void *);

	SkllCallbackHolder();
	int		invoke(SkllEvent &e) const;
	bool	valid() const;
};

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   INVOKERS - Type-specific callback invocation                              */
/* ═══════════════════════════════════════════════════════════════════════════ */

/* Invoker for callbacks WITHOUT user data: int cb(SkllEvent&) */
int skll_invoke_simple(void *fn, SkllEvent &e, void *data);

/* Template invoker for callbacks WITH typed user data: int cb(SkllEvent&, T*) */
template<typename T>
int skll_invoke_typed(void *fn, SkllEvent &e, void *data) {
	typedef int (*TypedFn)(SkllEvent &, T *);
	TypedFn typed_fn = reinterpret_cast<TypedFn>(fn);
	return typed_fn(e, static_cast<T *>(data));
}

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   CALLBACK CREATION HELPERS                                                 */
/* ═══════════════════════════════════════════════════════════════════════════ */

/* Create holder from simple callback (no user data) */
SkllCallbackHolder skll_cb(int (*fn)(SkllEvent &));

/* Create holder from typed callback with user data */
template<typename T>
SkllCallbackHolder skll_cb(int (*fn)(SkllEvent &, T *), T *data) {
	SkllCallbackHolder h;
	h.fn_ptr = reinterpret_cast<void *>(fn);
	h.user_data = static_cast<void *>(data);
	h.invoker = &skll_invoke_typed<T>;
	return h;
}

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   LEGACY CALLBACK TYPE - For backwards compatibility                        */
/* ═══════════════════════════════════════════════════════════════════════════ */

typedef int (*SkllCallback)(SkllEvent &);

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   HOOK - Single callback entry (wraps SkllCallbackHolder)                   */
/* ═══════════════════════════════════════════════════════════════════════════ */

struct SkllHook {
	SkllCallbackHolder	holder;

	SkllHook();
	SkllHook(SkllCallback fn);
	SkllHook(const SkllCallbackHolder &h);

	template<typename T>
	SkllHook(int (*fn)(SkllEvent &, T *), T *data) : holder(skll_cb(fn, data)) {}

	int		call(SkllEvent &e) const;
	bool	valid() const;
};

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   HOOKS - Collection of callbacks                                           */
/*   • Unlimited callbacks (std::deque)                                        */
/*   • SKLL_STOP halts chain                                                   */
/* ═══════════════════════════════════════════════════════════════════════════ */

class SkllHooks {
public:
	SkllHooks();
	~SkllHooks();

	/* Add simple callback */
	SkllHooks &		add(SkllCallback fn);

	/* Add typed callback with user data */
	template<typename T>
	SkllHooks &		add(int (*fn)(SkllEvent &, T *), T *data) {
		_hooks.push_back(SkllHook(fn, data));
		return *this;
	}

	/* Add from holder */
	SkllHooks &		add(const SkllCallbackHolder &h);

	/* Remove/clear */
	void			clear();

	/* State */
	bool			empty() const;
	size_t			size() const;

	/* Execute all - returns first non-OK/NEXT code, or SKLL_OK */
	int				run(SkllEvent &e) const;

	/* Access */
	const SkllHook &	at(size_t i) const;

private:
	std::deque<SkllHook>	_hooks;

	SkllHooks(const SkllHooks &);
	SkllHooks &operator=(const SkllHooks &);
};
