/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SkllClient.tpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/29 01:06:35 by marvin            #+#    #+#             */
/*   Updated: 2025/11/29 01:06:35 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <Sockell/SkllClient.hpp>

template <typename T> T			*SkllClient::data() { return (static_cast<T *>(_data)); }
template <typename T> void		SkllClient::set_data(T *ptr) { _data = ptr; }
template <typename T> const T	*SkllClient::data() const { return (static_cast<const T *>(_data)); }
