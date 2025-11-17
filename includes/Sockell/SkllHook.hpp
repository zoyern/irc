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
#include <map>
#include <cstddef>

class SkllHook {
	public:
		typedef void (*Callback)(void *lib_data, void *user_data);
	private:
		std::map<int, Callback>	_callbacks;
		std::map<int, void*>	_userdata;
	public:
		~SkllHook();
		SkllHook();
		SkllHook(const SkllHook &other);
		SkllHook	&operator=(const SkllHook &other);
		
		SkllHook	&on(int event, Callback callback, void *user_data = NULL);
		void		off(int event);
		void		trigger(int event, void *lib_data);
		void		clear();
		bool		has(int event) const;
		size_t		count() const;
};