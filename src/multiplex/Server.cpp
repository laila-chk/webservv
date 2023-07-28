/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mtellami <mtellami@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/22 18:11:22 by mtellami          #+#    #+#             */
/*   Updated: 2023/07/27 14:40:23 by mtellami         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

// <================================ THIS IS WEBSERV ================================>

Server::Server(void) {
    _nfds = 0;
    FD_ZERO(&_readfds);
    FD_ZERO(&_writefds);
}

Server::~Server(void) {
    std::vector<Cluster*>::iterator it;
    std::list<Client*>::iterator it1;

    for (it = _clusters.begin(); it != _clusters.end(); it++)
        delete *it;
    for (it1 = _clients.begin(); it1 != _clients.end(); it1++)
        delete *it1;
}

// Initialize Server blocks and set listening sockets
void Server::init(char *filename) {
    std::string                     path("src/parse/default.conf");
    std::vector<Config>             configs;
    SOCK_FD                         fd;
    std::vector<Config>::iterator   it;
    std::vector<Cluster*>::iterator _it;

    if (filename)
        path = std::string(filename);
    Serv_block_init(configs, path);
    for (it = configs.begin(); it != configs.end(); it++) {
        _clusters.push_back(new Cluster(*it));
    }
    for (_it = _clusters.begin(); _it != _clusters.end(); _it++) {
        fd = (*_it)->get_listen_fd();
        _nfds = (_nfds < fd) ? fd : _nfds;
        FD_SET(fd, &_readfds);
    }
}

// Set Sockets file descriptor for SELECT
void Server::set_fds() {
    SOCK_FD fd;
    std::list<Client*>::iterator it;

    FD_ZERO(&_copy_readfds);
    FD_ZERO(&_copy_writefds);
    _copy_readfds = _readfds;
    _copy_writefds = _writefds;
    for (it = _clients.begin(); it != _clients.end(); it++) {
        fd = (*it)->get_connect_fd();
        _nfds = (_nfds < fd) ? fd : _nfds;
    }
}

// Accept new client connection by Checking the ReadyToReadFrom (_readfds) list setted by select function
void Server::accept_connection() {
    std::vector<Cluster*>::iterator it;
    for (it = _clusters.begin(); it != _clusters.end(); it++) {
        if (FD_ISSET((*it)->get_listen_fd(), &_copy_readfds)) {
            Client  *client = new Client(*it);
            _clients.push_back(client);
            FD_SET(client->get_connect_fd(), &_readfds);
            FD_SET(client->get_connect_fd(), &_writefds);
            std::cout << "New client connected" << std::endl;
        }
    }
}

// Drop Served Client
void    Server::drop_client(Client *client) {
    std::list<Client*>::iterator it;
    close(client->get_connect_fd());
    FD_CLR(client->get_connect_fd(), &_readfds);
    FD_CLR(client->get_connect_fd(),  &_writefds);
    delete(client);
    for (it = _clients.begin(); it != _clients.end(); it++) {
        if ((*it)->get_connect_fd() == client->get_connect_fd())
            break;
    }
    if (it != _clients.end())
        _clients.erase(it);
    std::cout << "Client droped" << std::endl;
}

// Request
void    Server::handle_requests(void) {
    std::list<Client*>::iterator it;

    for (it = _clients.begin(); it != _clients.end(); it++) {
        if (FD_ISSET((*it)->get_connect_fd(), &_copy_readfds))
            (*it)->recieve();
    }
}

// Response
void    Server::responsing(void) {
    std::list<Client*>::iterator it;

    for (it = _clients.begin(); it != _clients.end(); it++) {
        if (FD_ISSET((*it)->get_connect_fd(), &_copy_writefds)) {
            (*it)->sending();
            if ((*it)->done_send())
                drop_client(*it);
        }
    }
}

// Run the Webserv
void Server::run(void) {
    std::cout << "Server listning on port 8080" << std::endl;

    while (true) {
        // multiplexing
        set_fds();
        if (select(_nfds + 1, &_copy_readfds, &_copy_writefds, NULL, 0) == FAIL)
            throw System();
        accept_connection();
        // request
        handle_requests();
        // response
        responsing();
    }
}

// void    Server::stop(void) {
//     std::vector<Cluster*>::iterator it;
//     std::list<Client*>::iterator    _it;

//     for (it = _clusters.begin(); it != _clusters.end(); it++)
//         close((*it)->get_listen_fd());
//     for (_it = _clients.begin(); _it != _clients.end(); _it++)
//         close((*_it)->get_connect_fd());
// }
