/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mtellami <mtellami@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/23 08:42:38 by mtellami          #+#    #+#             */
/*   Updated: 2023/08/03 11:44:20 by mtellami         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HP
#define CLIENT_HP

#include "include.hpp"
#include "Cluster.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "parsing.hpp"

class Response;

class Client {
    private:
        Cluster     *_cluster;
        Request     *_req;
        Response    *_res;
        locations   *_matched;
        SOCK_FD     _socket;
        bool        _done_recv;
        bool        _done_send;
        std::string _recv_buffer;
        std::string _send_buffer;

    public:
        Client(Cluster *cluster);
        ~Client(void);
        SOCK_FD get_connect_fd();
        void    recieve(void);
        void    sending(void);
        bool    done_send(void);
        void    parse_header(void);
        void    recv_body(void);
        void    get_matched_location(void);
};

#endif
