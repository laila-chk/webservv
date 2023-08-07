/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maamer <maamer@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/22 21:12:45 by mtellami          #+#    #+#             */
/*   Updated: 2023/08/05 11:37:25 by maamer           ###   ########.fr       */
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
    _bad_request = true;
}

// Destructor
Request::~Request() {
}

// Getter
std::string Request::get_method(void) {
    return _start_line[0];
}

bool Request::recieve_header(void) {
    return _recv_header;
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
    std::istringstream  _ss(_recv_buffer);
    std::string         buff;

    while (_ss >> buff)
        _start_line.push_back(std::string(buff));
    if (_start_line[1].find("?") != std::string::npos)
        _query = _start_line[1].substr(_start_line[1].find("?") + 1);
    while (std::getline(iss, line)) 
        _req_header.insert(_req_header.end(), std::make_pair(line.substr(0, line.find(":")), line.substr(line.find(" "))));
    _recv_buffer = "";
    if (_start_line[0] != "POST") {
        _done_recv = true;
        return ;
    }
    _buffer_size = 0;
    _body_size = _stoi(_req_header.find("Content-Length")->second);
}

// get the client request header
void    Request::get_request_header(SOCK_FD & _socket, bool & _done_recv) {
    while (_recv_buffer.find("\r\n\r\n") == std::string::npos) {
        _i = recv(_socket, _buffer, 1, 0);
        if (_i == FAIL)
            throw System();
        if (!_i) {
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

// write the readed chunk to the file
void Request::write_body_chunk(bool & _done_recv) {
    if (_done_recv)
        return;
    std::string suffix(_req_header.find("Content-Type")->second.substr(_req_header.find("Content-Type")->second.find("/") + 1));
    std::ofstream out;
	

		out.open(std::string("upload/" + _filename + "." + suffix).c_str(), std::ios::binary | std::ios::app);
    out << _recv_buffer;
    out.close();
    _recv_buffer = "";
}

// get the client request body by chunks
void Request::get_request_body(SOCK_FD & _socket, bool & _done_recv) {
    if (_done_recv)
        return ;
    if (_filename == "")
        _filename = rand_name();

    int i = 0;
    while (_buffer_size < (size_t)_body_size && i < SIZE) {
        _i = recv(_socket, _buffer, 1, 0);
        if (_i == FAIL)
            throw System();
        if (!_i) {
            _done_recv = true;
            return ;
        }
        _recv_buffer += std::string(_buffer, _i);
        i++;
        _buffer_size++;
    }
    write_body_chunk(_done_recv);
    if (_buffer_size == (size_t)_body_size)
        _done_recv = true;
}

// ----------- REQUEST HEADER EXAMPLE -------------------

// POST /cgi-bin/process.cgi HTTP/1.1
// User-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)
// Host: www.tutorialspoint.com
// Content-Type: application/x-www-form-urlencoded
// Content-Length: length
// Accept-Language: en-us
// Accept-Encoding: gzip, deflate
// Connection: Keep-Alive
