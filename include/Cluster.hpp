/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cluster.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mtellami <mtellami@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/22 20:18:06 by mtellami          #+#    #+#             */
/*   Updated: 2023/07/28 09:41:35 by mtellami         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SEVER_HPP
#define SEVER_HPP

#include "include.hpp"
#include "parsing.hpp"

class Cluster {
    private:
        Config              _config;
        SOCK_FD             _socket_fd;
        struct sockaddr_in  _address;
        socklen_t           _addrlen;

    public:
        Cluster(Config _conf);
        ~Cluster(void);

        SOCK_FD             get_listen_fd(void);
        struct sockaddr_in  *get_address(void);
        socklen_t           *get_addrlen(void);
        Config&             get_config(void);
};

#endif
