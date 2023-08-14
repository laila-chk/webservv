/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mtellami <mtellami@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/23 08:42:38 by mtellami          #+#    #+#             */
/*   Updated: 2023/08/14 09:29:57 by mtellami         ###   ########.fr       */
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
    _matched = NULL;
}

// Destructor
Client::~Client(void) {
    delete _req;
    delete _res;
    if (_matched)
      delete _matched;
}

// Getter
SOCK_FD Client::get_connect_fd() {
    return _socket;
}

bool    Client::done_send(void) {
    return _done_send;
}

Request  *Client::get_req() {
  return _req;
}

locations *Client::get_location() {
  return _matched;
}

// stoi
static int _stoi(std::string str) {
	std::istringstream iss(str);
	int nbr;
	iss >> nbr;
	return nbr;
}

// Recive from the ready client
void    Client::recieve(void) {
    if (_done_recv)
        return ;
  if (!_req->recieve_header()) {
      _req->get_request_header(_socket, _done_recv);
      get_matched_location();
      if (!_matched) {
        _done_recv = true;
        return;
      }
      std::vector<std::string>::iterator it;
      for (it = _matched->methods.begin(); it != _matched->methods.end(); it++) {
        if (*it == _req->get_method())
          break;
      }
      if (it == _matched->methods.end()) {
        _req->method_is_not_allowed(true);
        _done_recv = true;
        return;
      }
      if (_req->get_method() == "POST" && _stoi(_req->get_req_header().find("Content-Length")->second) \
        > _cluster->get_config().client_max_body_size) {
        _req->payload_is_too_large(true);
        _done_recv = true;
        return ;
      }
    } else {
      _req->get_request_body(_socket, _done_recv, _matched->root + _req->get_url());
    }
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
  search = "/";
  for (it = loc.begin(); it != loc.end(); it++) {
      if (it->pattern == search) {
        _matched = new locations(*it);
        return ;
      }
    }

  _done_recv = true;
}

// Response to the ready client
void    Client::sending(void) {
  if (!_done_recv || _done_send)
      return;
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
  if (!_matched || _req->is_not_found()) {
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
  // response
  if (_req->get_method() == "GET") {
    _res->GET(this);
  } else if (_req->get_method() == "POST") {
    _res->POST(this);
  } else if (_req->get_method() == "DELETE") {
    _res->DELETE(this);
  }
  _done_send = true;
}

