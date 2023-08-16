/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maamer <maamer@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/22 21:15:03 by mtellami          #+#    #+#             */
/*   Updated: 2023/08/16 16:44:44 by mtellami         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include <cstring>
Response::Response(Cluster *cluster) : _cluster(cluster) {
}

Response::~Response() {
}

// CPP11 std::to_string alternative
static std::string _to_string(int nbr) {
  std::stringstream  ss;
	ss << nbr;
	return ss.str();
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
			throw std::runtime_error("Unknown status code" + _to_string(status));
	}
}

// void Response::setBodySize(off_t size)
// {
// 	this->_body_size = size;
// }
void Response::toString(std::string const  &type)
{
	this->_header += "HTTP/1.1 ";
	this->_header += _to_string(this->_status_code) + " " + getStatusMsg(this->_status_code);
	this->_header += "\r\n";
	if (this->_location != "")
	{
		this->_header += "Location: " + this->_location + "\r\n";
		this->_header += std::string("Connection: close") + "\r\n\r\n";
		return ;
	}
	this->_header += "Content-Type: " + type + "\r\n";
	this->_header += "Content-Length: " + _to_string(this->_body_size) + "\r\n";
	this->_header += std::string("Connection: close") + "\r\n\r\n";
}


// int Response:: get_methode(Config &config_file)
// {

// }

std::string Response::get_error_page(std::string page, int code) {
  std::string line("HTTP/1.1 ");
  line += _to_string(code) + " ";
  line += getStatusMsg(code) + "\n";
  std::string res("Content-Type: text/html\nContent-Length: ");
  res = line + res;
  std::string content;
  char buffer[2];
  int fd = open(page.c_str(), O_RDONLY);
  if (fd == FAIL) {
    std::cerr << "Cant open the file" << std::endl;
    throw System();
  }
  while (true) {
    int i = read(fd, buffer, 1);
    if (i == FAIL)
      throw System();
    if (!i)
      break;
    content += std::string(buffer, i);
  }
  res += _to_string(content.length()) + "\n\n";
  res += content;
  return res;
}

void  Response::bad_request(Client *cl) {
    std::string res = get_error_page("src/response_pages/400.html", 400);
    send(cl->get_connect_fd(), res.c_str(), strlen(res.c_str()), 0);
}

void  Response::payload_too_large(Client *cl) {
  std::string res = get_error_page("src/response_pages/413.html", 413);
  send(cl->get_connect_fd(), res.c_str(), strlen(res.c_str()), 0);\
}

void  Response::not_found(Client *cl) {
  std::string res = get_error_page("src/response_pages/404.html", 404);
  send(cl->get_connect_fd(), res.c_str(), strlen(res.c_str()), 0);
}

void  Response::method_not_allowed(Client *cl) {
  std::string res = get_error_page("src/response_pages/405.html", 405);
  send(cl->get_connect_fd(), res.c_str(), strlen(res.c_str()), 0);
}
std::string Response::full_path(locations *var)
{
  std::string full_path = var->root + var->pattern ;
  return full_path;
}
bool Response::file_exists(const char *path) {
	return (access(path, F_OK) == 0);
}
bool Response::isDirectory(const char *path) {
	DIR* dir = opendir(path);
	if (dir != NULL)
	{
		closedir(dir);
		return true;
	}
	return false;
}

// get Request full path
std::string get_req_path(Client *cl) {
	std::string url = cl->get_req()->get_url();
	std::string pattern = cl->get_location()->pattern;
	std::string path = cl->get_location()->root + "/" + url.substr(url.find(pattern) + pattern.length());
	return path;
}

std::string get_full_path(Client *cl, std::string path) {
	std::string ret;
	for (std::vector<std::string>::iterator it = cl->get_location()->def_files.begin(); \
			it != cl->get_location()->def_files.end(); it++) {
		if (!access(std::string(path + "/" + *it).c_str(), F_OK)) {
			ret = path + "/" + *it;
			break;
		}
	}
	return ret;
}

std::string list_directory_content(Client *cl) {\
	(void)cl;
	std::string content = "HTTP/1.1 200 OK\n\
  Content-Type: text/html\n\
  Content-Length: 20\n\n\
  <h1 style=\"font-size:5rem\">FOLDER CONTENT</h1>";
	return content;
}

// main mathods
void  Response::GET(Client *cl) {
	std::string res;
	std::string path = get_req_path(cl);
	if (isDirectory(path.c_str())) {
		std::string full_path = get_full_path(cl, path);
		if (full_path != "") {
			res = get_error_page(full_path ,200);
		} else if (cl->get_location()->autoindex) {
			res = list_directory_content(cl);
		} else {
    	res = get_error_page("src/response_pages/404.html", 404);
		}
	} else if (!access(path.c_str(), F_OK)) {
    res = get_error_page(path, 200);
	} else {
    res = get_error_page("src/response_pages/404.html", 404);
	}
   send(cl->get_connect_fd(), res.c_str(), strlen(res.c_str()), 0);
}

// POST request
void Response::POST(Client *cl) {
  std::string res = get_error_page("src/response_pages/201.html", 201);
  send(cl->get_connect_fd(), res.c_str(), strlen(res.c_str()), 0);
}

// DELETE request
void Response::DELETE(Client *cl) {
  std::string url = cl->get_location()->root + cl->get_req()->get_url();
  std::string res;

  if (!access(url.c_str(), F_OK)) {
    if (!isDirectory(url.c_str())) {
			unlink(url.c_str());
      res = get_error_page("src/response_pages/200.html", 200);
    } else {
      res = get_error_page("src/response_pages/403.html", 403);
    }
  } else {
    res = get_error_page("src/response_pages/404.html", 404);
  }
  send(cl->get_connect_fd(), res.c_str(), strlen(res.c_str()), 0);
}

