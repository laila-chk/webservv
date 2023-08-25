/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maamer <maamer@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/22 21:12:45 by mtellami          #+#    #+#             */
/*   Updated: 2023/08/24 21:36:15 by mtellami         ###   ########.fr       */
/*   Updated: 2023/08/05 13:48:15 by mtellami         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

// Constructor
Request::Request() {
    _i = 0;
    _recv_header = false;
    _body_size = 0;
    _filename = "";
    _query = "";
    _bad_request = false;
    _payload_too_large = false;
    _method_not_allowed = false;
    _not_found = false;
}

// Destructor
Request::~Request() {
}

// Getters ...
std::string Request::get_method() {
    return _start_line[0];
}

std::string Request::get_url() {
  return _start_line[1];
}

bool Request::recieve_header() {
    return _recv_header;
}

bool  Request::is_bad_request() {
  return _bad_request;
}

bool  Request::is_payload_too_large() {
  return _payload_too_large;
}

bool  Request::is_not_found() {
  return _not_found;
}

std::string Request::getContentLength() {
  return _req_header.find("Content-Length")->second;
}

std::string Request::get_query(void){
  return _query;
}

std::string Request::get_protocol(void) {
    return _start_line[2];
}

std::map<std::string, std::string> Request::get_req_header(void) {
  return _req_header;
}

// Setters ...
void  Request::method_is_not_allowed(bool stat) {
  _method_not_allowed = stat;
}

void  Request::payload_is_too_large(bool stat) {
  _payload_too_large = stat;
}

// _stoi
static int _stoi(std::string str) {
	std::istringstream iss(str);
	int nbr;
	iss >> nbr;
	return nbr;
}

// parse the client request header
void    Request::parse_request_header(bool & _done_recv) {
    _recv_buffer = _recv_buffer.substr(0, _recv_buffer.find("\r\n\r\n"));
    std::istringstream  iss(_recv_buffer);
    std::string         line;

    std::getline(iss, line);
    std::istringstream  _ss(line);
    std::string         buff;

    while (_ss >> buff)
        _start_line.push_back(std::string(buff));
    if (_start_line.size() != 3) {
      _bad_request = true;
      _done_recv = true;
      return;
    }
    if (_start_line[1].find("?") != std::string::npos) {
      _query = _start_line[1].substr(_start_line[1].find("?") + 1);
      _start_line[1] = _start_line[1].substr(0, _start_line[1].find("?"));
    }
    while (std::getline(iss, line)) 
        _req_header.insert(_req_header.end(), std::make_pair(line.substr(0, line.find(":")), line.substr(line.find(" "))));
    _recv_buffer = "";
    if (_start_line[0] != "POST") {
        _done_recv = true;
        return ;
    }
    _buffer_size = 0;
    if ( _req_header.find("Content-Length") == _req_header.end()) {
      _bad_request = true;
      _done_recv = true;
      return;
    }
    _body_size = _stoi(_req_header.find("Content-Length")->second);
    if (!_body_size)
      _done_recv = true;
}

// get the client request header
void    Request::get_request_header(SOCK_FD & _socket, bool & _done_recv) {
    while (_recv_buffer.find("\r\n\r\n") == std::string::npos) {
        _i = recv(_socket, _buffer, 1, 0);
        if (_i == FAIL || !_i) {
            _done_recv = true;
            return;
        }
        _recv_buffer += std::string(_buffer, _i);
    }
    parse_request_header(_done_recv);
    _recv_header = true;
}

// Generate random file name
std::string rand_name(void) {
    char alphanum[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    std::string name;
    srand(time(0));

    for (int i = 0; i < 20; i++) {
        name += alphanum[rand() % (sizeof(alphanum) - 1)];
    }
    return name;
}

// get file extention from content type
std::string get_extention(std::string content) {
	std::string suffix;
	suffix = content.substr(content.find("/") + 1);
	if (suffix[suffix.length() - 1] == '\r')
		suffix = suffix.substr(0, suffix.length() -1);
	if (content.find("application/octet-stream") != std::string::npos)
		return "";
	else if (content.find("text/plain") != std::string::npos)
		suffix = "txt";
	std::cout << content << std::endl;
	return suffix + ".";
}

static bool is_directory(std::string path) {
	DIR* dir = opendir(path.c_str());
	if (dir) {
		closedir(dir);
		return true;
	}
	return false;
}

// write the readed chunk to the file
void Request::write_body_chunk(bool & _done_recv, std::string path) {
    if (_done_recv)
        return;
    std::string suffix = get_extention(_req_header.find("Content-Type")->second);
    std::ofstream out;

    if (!is_directory(path)) {
      _not_found = true;
      _done_recv = true;
      return ;
    }
    path += _filename + suffix;

		out.open(path.c_str(), std::ios::binary | std::ios::app);
    out << _recv_buffer;
    out.close();
    _recv_buffer = "";
}

bool	is_chunked_encoded(std::map<std::string, std::string> _req_header) {
	std::map<std::string, std::string>::iterator it;
	for (it = _req_header.begin(); it != _req_header.end(); it++) {
		if (it->first == "Transfer-Encoding" && it->second.find("chunked") != std::string::npos)
			return true;
	}
	return false;
}

void	Request::transfer_encoding(SOCK_FD & _socket, bool & _done_recv, std::string path) {
	int i = 0;
	if (_filename == "")
		_filename = rand_name();
	while (i < SIZE) {
	// get chunkn size
		std::string line;
		for (;;) {
			_i = recv(_socket, _buffer, 1, 0);
			if (_i == FAIL || !_i) {
				_done_recv = true;
				return ;
			}
			if (line.find("\r\n") != std::string::npos)
				break;
			line += std::string(_buffer, _i);
		}
		std::stringstream ss(line);
		int chunkSize;
		ss >> std::hex >> chunkSize;
		if (!chunkSize) {
			_done_recv = true;
			return ;
		}
		// read the chuunk
		_recv_buffer += std::string(_buffer, _i);
		for (int j = 0; j < chunkSize; j++) {
			_i = recv(_socket, _buffer, 1, 0);
			if (_i == FAIL || !_i) {
				_done_recv = true;
				return;
			}
  	  _recv_buffer += std::string(_buffer, _i);
		}
		i += chunkSize;
		// write to body
		write_body_chunk(_done_recv, path);
	}
}

// get the client request body by chunks
void Request::get_request_body(SOCK_FD & _socket, bool & _done_recv, std::string path) {
    if (_done_recv)
        return ;
		int i = 0;
		if (is_chunked_encoded(_req_header)) {
			transfer_encoding(_socket, _done_recv, path);
			return;
		}
    if (_filename == "")
        _filename = rand_name();

    while (_buffer_size < (size_t)_body_size && i < SIZE) {
        _i = recv(_socket, _buffer, 1, 0);
        if (_i == FAIL || !_i) {
            _done_recv = true;
            return ;
        }
        _recv_buffer += std::string(_buffer, _i);
        i++;
        _buffer_size++;
    }
    write_body_chunk(_done_recv, path);
    if (_buffer_size == (size_t)_body_size)
        _done_recv = true;
}

