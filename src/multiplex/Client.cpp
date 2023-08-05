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

#include "Client.hpp"

// Client constructor
Client::Client(Cluster *cluster) : _cluster(cluster) {
    _done_recv = false;
    _done_send = false;
    _socket = accept(_cluster->get_listen_fd(), (struct sockaddr *)_cluster->get_address(), (socklen_t*)_cluster->get_addrlen());
    _req = new Request;
    _res = new Response(cluster); 
}

// Destructor
Client::~Client(void) {
    delete _req;
    delete _res;
}

// Getter
SOCK_FD Client::get_connect_fd() {
    return _socket;
}

bool    Client::done_send(void) {
    return _done_send;
}

// Recive from the ready client
void    Client::recieve(void) {
    if (_done_recv)
        return ;
    if (!_req->recieve_header())
        _req->get_request_header(_socket, _done_recv);
    else
        _req->get_request_body(_socket, _done_recv);
}

// Response to the ready client
void    Client::sending(void) {
    if (!_done_recv)
        return;

    std::string res("HTTP/1.1 200 OK\n\
    Content-Type: text/html\n\
    Content-Length: 20\n\n\
    <h1 style=\"font-size:5rem\"> request Accepted and served</h1>");

    send(_socket, res.c_str(), strlen(res.c_str()), 0);
    _done_send = true;
}

// TODO:  parse the path in the request 