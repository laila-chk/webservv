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
    _res = new Response;
    _matched = NULL;
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

// set the mathced location
void  Client::get_matched_location() {
  std::vector<locations>::iterator it;
  std::vector<locations> loc = _cluster->get_config().loc;
  std::string search = _req->get_url();
  while (search != "") {
    for (it = loc.begin(); it != loc.end(); it++) {
      if (it->pattern == search) {
        _matched = new locations(*it);
        return ;
      }
    }
    size_t pos = search.find_last_of("/");
    if (pos == std::string::npos)
        break;
    search = search.substr(0, pos);
  }
}

// Response to the ready client
void    Client::sending(void) {
    if (!_done_recv || _done_send)
        return;
  // this kindaa hardcode i'll change it latter
  if (_req->is_bad_request()) {
    _res->bad_request(this);
    _done_send = true;
    return ;
  }
  if (_req->is_payload_too_large()) {
    _res->payload_too_large(this);
    _done_send = true;
    return ;
  }
  // 404 not founf if not match a location
  get_matched_location();
  if (!_matched) {
    _res->not_found(this);
    _done_send = true;
    return ;
  }
  std::vector<std::string>::iterator it;
  for (it = _matched->methods.begin(); it != _matched->methods.end(); it++) {
    if (*it == _req->get_method())
      break;
  }
  if (it == _matched->methods.end()) {
    _res->method_not_allowed(this);
    _done_send = true;
    return ;
  }
  // 405 method not allowed (need to be checked after finding the appropriate location)
  // response
  if (_req->get_method() == "GET") {
    _res->GET(this);
  } else if (_req->get_method() == "POST") {
    _res->POST(this);
  } else if (_req->get_method() == "DELETE") {
    _res->DELETE(this);
  }
    std::string res("HTTP/1.1 200 OK\n\
    Content-Type: text/html\n\
    Content-Length: 20\n\n\
    <h1 style=\"font-size:5rem\"> request Accepted and served</h1>");

    send(_socket, res.c_str(), strlen(res.c_str()), 0);
    _done_send = true;
}

