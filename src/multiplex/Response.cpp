/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maamer <maamer@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/22 21:15:03 by mtellami          #+#    #+#             */
/*   Updated: 2023/08/08 11:29:53 by maamer           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
// Response::Response(Cluster *cluster) : _cluster(cluster) {
// }

Response::~Response() {
}


//The purpose of this function is to determine the appropriate Content-Type 
//The code extracts the file extension from the _path variable of the Request class
std::string Request::getContentType()
{
    std::map<std::string, std::string> content;
    content["txt"] = "text/plain";
    content["html"] = "text/html";
    content["css"] = "text/css";
    content["js"] = "text/javascript";
    content["json"] ="application/json";
    content["xml"] = "application/xml";
    content["pdf"] = "application/pdf";
    content["zip"] = "application/zip";
    content["jpeg"] = "image/jpeg";
    content["png"] = "image/png";
    content["gif"] = "image/gif";
    content["ogg"] = "audio/ogg";
    content["wav"] = "audio/wav";
    content["mp3"] = "audio/mp3";
    content["webm"] = "video/webm";
	content["mp4"] = "video/mp4";
    content[""] = "application/octet-stream";
    if (content.find(_start_line[1].substr(_start_line[1].find_last_of(".") + 1)) != content.end())
		return content[_start_line[1].substr(_start_line[1].find_last_of(".") + 1)];
	else
		return content[""];

}

std::string Response::getStatusMsg(int status)
{
    switch(status)
	{
		case OK:
			return "OK";
		case CREATED:
			return "Created";
		case NO_CONTENT:
			return "No Content";
		case BAD_REQUEST:
			return "Bad Request";
		case FORBIDDEN:
			return "Forbidden";
		case NOT_FOUND:
			return "Not Found";
        case MOVED_PERMANENTLY:
            return "Moved permanently";
		case NOT_IMPLEMENTED:
			return "Not Implemented";
		case METHOD_NOT_ALLOWED:
			return "Not Allowed";
		case REQUEST_ENTITY_TOO_LARGE:
			return "Request Entity Too Large";
		case REQUEST_URI_TOO_LARGE:
			return "Request URI Too Large";
		default:
			throw std::runtime_error("Unknown status code" + std::to_string(status));
	}
}

// void Response::setBodySize(off_t size)
// {
// 	this->_body_size = size;
// }
void Response::toString(std::string const  &type)
{
	this->_header += "HTTP/1.1 ";
	this->_header += std::to_string(this->_status_code) + " " + getStatusMsg(this->_status_code);
	this->_header += "\r\n";
	if (this->_location != "")
	{
		this->_header += "Location: " + this->_location + "\r\n";
		this->_header += std::string("Connection: close") + "\r\n\r\n";
		return ;
	}
	this->_header += "Content-Type: " + type + "\r\n";
	this->_header += "Content-Length: " + std::to_string(this->_body_size) + "\r\n";
	this->_header += std::string("Connection: close") + "\r\n\r\n";
}


// int Response:: get_methode(Config &config_file)
// {

// }

void  Response::GET(Client *cl) {
  (void)cl;
}

void Response::POST(Client *cl) {
  (void)cl;
}

void Response::DELETE(Client *cl) {
  (void)cl;
}

